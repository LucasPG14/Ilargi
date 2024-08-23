#include "ilargipch.h"

#include "EditorPanel.h"
#include "EditorPanels/SceneHierarchyInspectorPanel.h"
#include "EditorPanels/ResourcesPanel.h"

#include "Localization.h"

#include "Utils/FileSystem.h"
#include "Utils/Importers/SceneImporter.h"

#include <imgui/imgui.h>
#include <ImGuizmo.h>
#include <arduinojson/ArduinoJson-v7.0.4.h>

namespace Ilargi
{
	template<>
	vec2& vec2::operator=(const ImVec2& v)
	{
		x = v.x;
		y = v.y;
		
		return *this;
	}

	template<>
	bool vec2::operator!=(const ImVec2& v)
	{
		return (x != v.x || y != v.y);
	}

	static std::unordered_map<Texts, std::string> menuNames = {};

	EditorPanel::EditorPanel() : Panel("Editor Panel"), hierarchyInspector(nullptr), resourcesPanel(nullptr), 
		viewportSize({ 1080, 720 }), needToUpdateFramebuffer(false), constants(), operation(ImGuizmo::TRANSLATE)
	{
	}

	EditorPanel::~EditorPanel()
	{
	}

	void EditorPanel::OnInit()
	{
		hierarchyInspector = new SceneHierarchyInspectorPanel();
		resourcesPanel = new ResourcesPanel();
		
		NewScene();

		commandBuffer = CommandBuffer::Create(Renderer::GetConfig().maxFrames);
		
		framebuffer = Framebuffer::Create({ 1080, 720, { ImageFormat::RGBA8, ImageFormat::DEPTH32 }, false, true });
		{
			PipelineProperties pipelineProperties;
			pipelineProperties.name = "Geometry";
			pipelineProperties.shader = Renderer::GetShaderLibrary()->Get("PBR_Static");
			pipelineProperties.depth = true;
			pipelineProperties.layout =
			{
				{ ShaderDataType::FLOAT3, "position" },
				{ ShaderDataType::FLOAT3, "normal" },
				{ ShaderDataType::FLOAT3, "tangent" },
				{ ShaderDataType::FLOAT3, "bitangent" },
				{ ShaderDataType::FLOAT2, "texCoord" },
			};

			renderPass = RenderPass::Create({ framebuffer, Pipeline::Create(pipelineProperties), true });
		}

		PipelineProperties pipelineProperties;
		pipelineProperties.name = "Grid";
		//pipelineProperties.shader = Renderer::GetShaderLibrary()->Get("Grid");
		pipelineProperties.depth = true;
		pipelineProperties.layout = {};

		//gridRenderPass = RenderPass::Create({ framebuffer, Pipeline::Create(pipelineProperties), false });
		
		uboCamera = UniformBuffer::Create(sizeof(mat4), Renderer::GetConfig().maxFrames);

		LoadLanguage("Engine/Localization/english.json");
	}

	void EditorPanel::OnDestroy()
	{
		delete hierarchyInspector;
		delete resourcesPanel;

		ResourceManager::Clear();

		uboCamera->Destroy();
		
		scene->Destroy();

		framebuffer->Destroy();
		//gridRenderPass->Destroy();
		renderPass->Destroy();

		commandBuffer->Destroy();
	}

	void EditorPanel::Update()
	{
		if (needToUpdateFramebuffer)
		{
			framebuffer->Resize(renderPass, (uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			camera.Resize(viewportSize.x, viewportSize.y);
			needToUpdateFramebuffer = false;
		}

		camera.Update();

		commandBuffer->BeginCommand();
		renderPass->BeginRenderPass(commandBuffer);
		
		constants[0] = camera.GetProjectionMatrix() * camera.GetViewMatrix();

		auto ent = *scene->GetWorld().view<TransformComponent, DirectionalLightComponent>().begin();

		auto [trans, light] = scene->GetWorld().view<TransformComponent, DirectionalLightComponent>().get<>(ent);

		const auto& view = scene->GetWorld().view<TransformComponent, StaticMeshComponent>();
		for (auto entity : view)
		{
			auto [transform, meshComponent] = view.get<TransformComponent, StaticMeshComponent>(entity);

			auto mesh = meshComponent.staticMesh.lock();
			if (!mesh)
				continue;

			transform.CalculateTransform();
			
			renderPass->GetProperties().pipeline->Bind(commandBuffer);
			renderPass->GetProperties().pipeline->BindDescriptorSet(commandBuffer, mesh->GetMaterial());
			renderPass->GetProperties().pipeline->PushConstants(commandBuffer, 0, 64, transform.transform);
			renderPass->GetProperties().pipeline->PushConstants(commandBuffer, 64, 64, constants[0]);
			//renderPass->GetProperties().pipeline->PushConstants(commandBuffer, 128, 16, mesh.staticMesh->GetColor());
			renderPass->GetProperties().pipeline->PushConstants(commandBuffer, 144, 12, light.radiance);
			renderPass->GetProperties().pipeline->PushConstants(commandBuffer, 156, 12, trans.rotation);
			Renderer::SubmitGeometry(commandBuffer, mesh);
		}

		renderPass->EndRenderPass(commandBuffer);

		//gridRenderPass->BeginRenderPass(commandBuffer);
		//
		//gridRenderPass->GetProperties().pipeline->Bind(commandBuffer);
		//gridRenderPass->GetProperties().pipeline->PushConstants(commandBuffer, 0, 64, camera.GetViewMatrix());
		//gridRenderPass->GetProperties().pipeline->PushConstants(commandBuffer, 64, 64, camera.GetProjectionMatrix());
		//
		//Renderer::DrawDefault(commandBuffer);
		//
		//gridRenderPass->EndRenderPass(commandBuffer);

		commandBuffer->EndCommand();
		commandBuffer->Submit();
	}

	void EditorPanel::RenderImGui()
	{
		static bool dockspaceOpen = true;
		static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
			windowFlags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, windowFlags);
		ImGui::PopStyleVar(3);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID id = ImGui::GetID("Dockspace");
			ImGui::DockSpace(id, { 0.0f, 0.0f }, dockspaceFlags);
		}

		RenderMainMenuBar();

		RenderViewport();

		hierarchyInspector->Render();
		resourcesPanel->Render();

		ImGui::End();
	}
	
	void EditorPanel::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<KeyPressedEvent>(ILG_BIND_FN(EditorPanel::OnKeyEvent));

		resourcesPanel->OnEvent(event);
	}
	
	void EditorPanel::LoadLanguage(std::filesystem::path path)
	{
		std::ifstream file(path, std::ios::in);

		JsonDocument document;
		deserializeJson(document, file);

		menuNames[Texts::FILE] = document["File"].as<std::string>();
		menuNames[Texts::NEW_SCENE] = document["New Scene"].as<std::string>();
		menuNames[Texts::OPEN_SCENE] = document["Open Scene"].as<std::string>();
		menuNames[Texts::SAVE_SCENE] = document["Save Scene"].as<std::string>();
		menuNames[Texts::SAVE_SCENE_AS] = document["Save Scene As"].as<std::string>();
		menuNames[Texts::EXIT] = document["Exit"].as<std::string>();
		
		menuNames[Texts::EDIT] = document["Edit"].as<std::string>();
		menuNames[Texts::UNDO] = document["Undo"].as<std::string>();
		menuNames[Texts::REDO] = document["Redo"].as<std::string>();
		menuNames[Texts::COPY] = document["Copy"].as<std::string>();
		menuNames[Texts::PASTE] = document["Paste"].as<std::string>();
		menuNames[Texts::DELETE] = document["Delete"].as<std::string>();
		menuNames[Texts::DUPLICATE] = document["Duplicate"].as<std::string>();
		
		menuNames[Texts::LOCALIZATION] = document["Localization"].as<std::string>();
		menuNames[Texts::ENGLISH] = document["English"].as<std::string>();
		menuNames[Texts::SPANISH] = document["Spanish"].as<std::string>();
	}

	void EditorPanel::RenderMainMenuBar()
	{
		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu(menuNames[Texts::FILE].c_str()))
		{
			if (ImGui::MenuItem(menuNames[Texts::NEW_SCENE].c_str(), "Ctrl + N"))
			{
				NewScene();
			}
			if (ImGui::MenuItem(menuNames[Texts::OPEN_SCENE].c_str(), "Ctrl + O"))
			{
				OpenScene();
			}
			ImGui::Separator();
			if (ImGui::MenuItem(menuNames[Texts::SAVE_SCENE].c_str(), "Ctrl + S"))
			{
				// TODO: Change this to save the scene with the current path of the scene
				SaveSceneAs();
			}
			if (ImGui::MenuItem(menuNames[Texts::SAVE_SCENE_AS].c_str(), "Ctrl + Shift + S"))
			{
				SaveSceneAs();
			}
			ImGui::Separator();
			if (ImGui::MenuItem(menuNames[Texts::EXIT].c_str(), "Ctrl + Alt + F4"))
			{
				Application::Get()->CloseApp();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu(menuNames[Texts::EDIT].c_str()))
		{
			if (ImGui::MenuItem(menuNames[Texts::UNDO].c_str(), "Ctrl + Z"))
			{
				// TODO: Undo
			}
			if (ImGui::MenuItem(menuNames[Texts::REDO].c_str(), "Ctrl + Y"))
			{
				// TODO: Redo
			}
			ImGui::Separator();

			bool enabled = hierarchyInspector->GetSelected() != entt::null ? true : false;
			if (ImGui::MenuItem(menuNames[Texts::COPY].c_str(), "Ctrl + C", (bool*)0, enabled))
			{
				// TODO: Copy
			}
			if (ImGui::MenuItem(menuNames[Texts::PASTE].c_str(), "Ctrl + V", (bool*)0, enabled))
			{
				// TODO: Paste
			}
			if (ImGui::MenuItem(menuNames[Texts::DELETE].c_str(), "Del", (bool*)0, enabled))
			{
				scene->DestroyEntity(hierarchyInspector->GetSelected());
				hierarchyInspector->ResetSelected();
			}
			if (ImGui::MenuItem(menuNames[Texts::DUPLICATE].c_str(), "Ctrl + D", (bool*)0, enabled))
			{
				// TODO: Duplicate an entity
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu(menuNames[Texts::LOCALIZATION].c_str()))
		{
			if (ImGui::MenuItem(menuNames[Texts::ENGLISH].c_str()))
			{
				LoadLanguage("Engine/Localization/english.json");
			}
			if (ImGui::MenuItem(menuNames[Texts::SPANISH].c_str()))
			{
				LoadLanguage("Engine/Localization/spanish.json");
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	void EditorPanel::RenderViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::Begin("Viewport", 0, ImGuiWindowFlags_NoDecoration);
		ImVec2 frameViewportSize = ImGui::GetContentRegionAvail();

		ImGui::Image(framebuffer->GetID(), frameViewportSize, { 0.0f, 1.0f }, { 1.0f, 0.0f });

		if (viewportSize != frameViewportSize)
		{
			viewportSize = frameViewportSize;
			needToUpdateFramebuffer = true;
		}

		Entity entity = hierarchyInspector->GetSelected();
		// Guizmo
		if (entity != entt::null)
		{
			ImGuizmo::Enable(true);
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetGizmoSizeClipSpace(0.15f);

			const mat4& viewMatrix = camera.GetViewMatrix();
			const mat4& projMatrix = camera.GetProjectionMatrix();

			TransformComponent& transformComp = scene->GetWorld().get<TransformComponent>(entity);
			mat4& transform = transformComp.transform;

			ImGuizmo::Manipulate(viewMatrix, projMatrix, (ImGuizmo::OPERATION)operation, ImGuizmo::WORLD, transform);

			if (ImGuizmo::IsUsingAny())
			{
				ImGuizmo::DecomposeMatrixToComponents(transform, transformComp.position, transformComp.rotation, transformComp.scale);
			}
		}

		if (ImGui::BeginDragDropTarget())
		{
			auto payload = ImGui::AcceptDragDropPayload("RESOURCE");

			if (payload)
			{
				// TODO: Drag and drop from resource panel to viewport
				UUID uuid = *(UUID*)payload->Data;
				auto metadata = ResourceManager::GetResourcesMetadata()[uuid];

				switch (metadata.type)
				{
				case ResourceType::MODEL:
				{
					std::shared_ptr<Resource> resource = ResourceManager::GetResource(uuid);

					Entity entity = scene->CreateEntity();
					scene->CreateComponent<StaticMeshComponent>(entity, std::static_pointer_cast<StaticMesh>(resource));
					break;
				}
				case ResourceType::SCENE:
				{
					std::shared_ptr<Resource> resource = ResourceManager::GetResource(uuid);

					scene = std::static_pointer_cast<Scene>(resource);
					hierarchyInspector->SetScene(scene);
					break;
				}
				}
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorPanel::NewScene()
	{
		scene = std::make_shared<Scene>();
		hierarchyInspector->SetScene(scene);

		Entity entity = scene->CreateEntity("Directional Light");
		scene->CreateComponent<DirectionalLightComponent>(entity);
	}

	void EditorPanel::OpenScene()
	{
		//std::string filepath = FileSystem::OpenFile("Ilargi Scene (.ilargi)\0*.ilargi\0");
		//if (!filepath.empty())
		//	OpenScene(filepath);
	}

	void EditorPanel::OpenScene(std::string filepath)
	{
		std::shared_ptr<Scene> newScene = std::make_shared<Scene>();
	}

	void EditorPanel::SaveSceneAs()
	{
		std::string filepath = FileSystem::SaveFile("Ilargi Scene (.ilargi)\0*.ilargi\0");
		if (!filepath.empty())
		{
			SaveScene(filepath);
			resourcesPanel->RefreshAssets();
		}
	}

	void EditorPanel::SaveScene(std::string filepath)
	{
		SceneImporter::SaveScene(scene, filepath);

		// TODO: Think a better way of handle this if possible
		auto start = filepath.find("Resources");
		ResourceManager::ImportResource(std::filesystem::path(filepath.substr(start)).remove_filename(), std::filesystem::path(filepath.substr(start)));

		ResourceManager::SaveResourceRegistry();
	}
	
	bool EditorPanel::OnKeyEvent(KeyPressedEvent& event)
	{
		bool ctrl = Input::IsKeyPressed(KeyCode::LEFT_CONTROL) || Input::IsKeyPressed(KeyCode::RIGHT_CONTROL);
		bool shift = Input::IsKeyPressed(KeyCode::LEFT_SHIFT) || Input::IsKeyPressed(KeyCode::RIGHT_SHIFT);
		bool alt = Input::IsKeyPressed(KeyCode::LEFT_ALT) || Input::IsKeyPressed(KeyCode::RIGHT_ALT);

		switch (event.GetKey())
		{
		case KeyCode::N:
			if (ctrl)
			{
				NewScene();
			}
			break;
		case KeyCode::O:
			if (ctrl)
			{
				OpenScene();
			}
			break;
		case KeyCode::S:
			if (ctrl)
			{
				if (shift)
				{
					SaveSceneAs();
					break;
				}
				// TODO: Change this to save the scene with the current path of the scene
				SaveSceneAs();
			}
			break;
		case KeyCode::W:
			if (!Input::IsMouseButtonPressed(MouseCode::RIGHT))
				operation = ImGuizmo::TRANSLATE;
			break;
		case KeyCode::E:
			if (!Input::IsMouseButtonPressed(MouseCode::RIGHT))
				operation = ImGuizmo::ROTATE;
			break;
		case KeyCode::R:
			if (!Input::IsMouseButtonPressed(MouseCode::RIGHT))
				operation = ImGuizmo::SCALE;
			break;
		case KeyCode::F4:
			Application::Get()->CloseApp();
			break;
		}

		return true;
	}
}