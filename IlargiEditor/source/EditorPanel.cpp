#include "ilargipch.h"

#include "EditorPanel.h"
#include "EditorPanels/SceneHierarchyInspectorPanel.h"
#include "EditorPanels/ResourcesPanel.h"

#include "Resources/Mesh.h"

#include "Utils/Importers/ModelImporter.h"

#include <imgui/imgui.h>
#include <ImGuizmo.h>

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

	EditorPanel::EditorPanel() : Panel("Editor Panel"), hierarchyInspector(nullptr), resourcesPanel(nullptr), 
		viewportSize({ 1080, 720 }), needToUpdateFramebuffer(false), constants(), operation(ImGuizmo::TRANSLATE)
	{
	}

	EditorPanel::~EditorPanel()
	{
	}

	void EditorPanel::OnInit()
	{
		scene = std::make_shared<Scene>();
		hierarchyInspector = new SceneHierarchyInspectorPanel(scene);
		resourcesPanel = new ResourcesPanel();

		commandBuffer = CommandBuffer::Create(Renderer::GetConfig().maxFrames);
		
		framebuffer = Framebuffer::Create({ 1080, 720, { ImageFormat::RGBA8, ImageFormat::DEPTH32 }, false });
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

			renderPass = RenderPass::Create({ framebuffer, Pipeline::Create(pipelineProperties) });
			
			const auto& view = scene->GetWorld().view<TransformComponent, StaticMeshComponent>();
			for (auto entity : view)
			{
				view.get<StaticMeshComponent>(entity).staticMesh->CreateMaterial(pipelineProperties.shader);
			}
		}

		//PipelineProperties pipelineProperties;
		//pipelineProperties.name = "Grid";
		//pipelineProperties.shader = Renderer::GetShaderLibrary()->Get("Grid");
		//pipelineProperties.depth = true;
		//pipelineProperties.layout = {};

		//gridRenderPass = RenderPass::Create({ framebuffer, Pipeline::Create(pipelineProperties) });
		
		uboCamera = UniformBuffer::Create(sizeof(mat4), Renderer::GetConfig().maxFrames);

	}

	void EditorPanel::OnDestroy()
	{
		uboCamera->Destroy();
		
		scene->Destroy();

		framebuffer->Destroy();
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

		const auto& view = scene->GetWorld().view<TransformComponent, StaticMeshComponent>();
		for (auto entity : view)
		{
			auto [transform, mesh] = view.get<TransformComponent, StaticMeshComponent>(entity);

			transform.CalculateTransform();
			
			renderPass->GetProperties().pipeline->Bind(commandBuffer);
			renderPass->GetProperties().pipeline->BindDescriptorSet(commandBuffer, mesh.staticMesh->GetMaterial());
			renderPass->GetProperties().pipeline->PushConstants(commandBuffer, 0, 64, transform.transform);
			renderPass->GetProperties().pipeline->PushConstants(commandBuffer, 64, 64, constants[0]);
			renderPass->GetProperties().pipeline->PushConstants(commandBuffer, 128, 16, mesh.staticMesh->GetColor());
			Renderer::SubmitGeometry(commandBuffer, mesh.staticMesh);
		}

		renderPass->EndRenderPass(commandBuffer);

		//gridRenderPass->BeginRenderPass(commandBuffer);
		
		//gridRenderPass->GetProperties().pipeline->Bind(commandBuffer);
		//gridRenderPass->GetProperties().pipeline->PushConstants(commandBuffer, 0, 64, camera.GetViewMatrix());
		//gridRenderPass->GetProperties().pipeline->PushConstants(commandBuffer, 64, 64, camera.GetProjectionMatrix());
		
		//Renderer::DrawDefault(commandBuffer);
		
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

		MainMenuBar();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::Begin("Viewport");
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

		ImGui::End();
		ImGui::PopStyleVar();

		hierarchyInspector->Render();
		resourcesPanel->Render();

		ImGui::End();
	}
	
	void EditorPanel::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<KeyPressedEvent>(ILG_BIND_FN(EditorPanel::OnKeyEvent));
	}
	
	void EditorPanel::MainMenuBar()
	{
		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene", "Ctrl + N"))
			{
				// TODO: New scene
			}
			if (ImGui::MenuItem("Open Scene", "Ctrl + O"))
			{
				// TODO: Open scene
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Save Scene", "Ctrl + S"))
			{
				// TODO: Save scene
			}
			if (ImGui::MenuItem("Save Scene As...", "Ctrl + Shift + S"))
			{
				// TODO: Save scene as...
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "Ctrl + Alt + F4"))
			{
				// TODO: Exit application
				Application::Get()->CloseApp();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "Ctrl + Z"))
			{
				// TODO: Undo
			}
			if (ImGui::MenuItem("Redo", "Ctrl + Y"))
			{
				// TODO: Redo
			}
			ImGui::Separator();

			bool enabled = hierarchyInspector->GetSelected() != entt::null ? true : false;
			if (ImGui::MenuItem("Copy", "Ctrl + C", (bool*)0, enabled))
			{
				// TODO: Copy
			}
			if (ImGui::MenuItem("Paste", "Ctrl + V", (bool*)0, enabled))
			{
				// TODO: Paste
			}
			if (ImGui::MenuItem("Delete", "Del", (bool*)0, enabled))
			{
				// TODO: Delete an entity
			}
			if (ImGui::MenuItem("Duplicate", "Ctrl + D", (bool*)0, enabled))
			{
				// TODO: Duplicate an entity
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
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
				// TODO: New scene
			}
			break;
		case KeyCode::O:
			if (ctrl)
			{
				// TODO: Open scene
			}
			break;
		case KeyCode::S:
			if (ctrl)
			{
				if (shift)
				{
					// TODO: Save scene as...
					break;
				}
				// TODO: Save scene
			}
			break;
		case KeyCode::W:
			operation = ImGuizmo::TRANSLATE;
			break;
		case KeyCode::E:
			operation = ImGuizmo::ROTATE;
			break;
		case KeyCode::R:
			operation = ImGuizmo::SCALE;
			break;
		case KeyCode::F4:
			Application::Get()->CloseApp();
			break;
		}

		return true;
	}
}