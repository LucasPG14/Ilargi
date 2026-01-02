#include "ilargipch.h"

#include "EditorPanel.h"
#include "EditorPanels/SceneHierarchyInspectorPanel.h"
#include "EditorPanels/ResourcesPanel.h"

#include "Localization.h"

#include "Resources/Model.h"
#include "Utils/FileSystem.h"
#include "Utils/Importers/SceneImporter.h"

#include <imgui/imgui.h>
#include <ImGuizmo.h>
#include <arduinojson/ArduinoJson-v7.0.4.h>
#include <gtc/type_ptr.hpp>

namespace Ilargi
{
	bool operator==(const glm::vec2& v, const ImVec2& v2)
	{
		return v.x == v2.x && v.y == v2.y;
	}

	bool operator!=(const glm::vec2& v, const ImVec2& v2)
	{
		return v.x != v2.x || v.y != v2.y;
	}

	static std::unordered_map<Texts, std::string> menuNames = {};

	EditorPanel::EditorPanel() : Panel("Editor Panel"), mHierarchyInspector(nullptr), mResourcesPanel(nullptr),
		mViewportSize({ 1080, 720 }), mOperation(ImGuizmo::TRANSLATE), mNeedToUpdateFramebuffer(false), mEditorMode(EditorMode::EDITOR), mStencilMatrix(1.0)
	{
	}

	EditorPanel::~EditorPanel()
	{
	}

	void EditorPanel::OnInit()
	{
		mHierarchyInspector = new SceneHierarchyInspectorPanel();
		mResourcesPanel = new ResourcesPanel();
		
		NewScene();

		mCommandBuffer = CommandBuffer::Create(Renderer::GetConfig().maxFrames);
		
		mRenderPass = RenderPass::Create({ { ImageFormat::RGBA8, ImageFormat::DEPTH24_STENCIL8 }, true });
		mFramebuffer = Framebuffer::Create({ { ImageFormat::RGBA8, ImageFormat::DEPTH24_STENCIL8 }, mRenderPass, 1080U, 720U, false, true });
		{
			PipelineProperties pipelineProperties
			{
				"Geometry",											// name
				mRenderPass,										// renderPass
				Renderer::GetShader("PBR_Static"),	// shader
				{													// layout
					{ ShaderDataType::FLOAT3_32, "position" },
					{ ShaderDataType::FLOAT3_32, "normal" },
					{ ShaderDataType::FLOAT3_32, "tangent" },
					{ ShaderDataType::FLOAT3_32, "bitangent" },
					{ ShaderDataType::FLOAT2_32, "texCoord" },
				},
				true,												// testDepth
				true,												// writeDepth
				true,												// hasStencil
				true,												// writeStencil
			};

			mGeometryPipeline = Pipeline::Create(pipelineProperties);
		}

		{
			PipelineProperties pipelineProperties
			{
				"Grid",										// name
				mRenderPass,								// renderPass
				Renderer::GetShader("Grid"),	// shader
				{},											// layout
				true,										// testDepth
				false,										// writeDepth
				false,										// hasStencil
				false,										// writeStencil
			};

			mGridPipeline = Pipeline::Create(pipelineProperties);
		}

		{
			PipelineProperties pipelineProperties
			{
				"Outline",											// name
				mRenderPass,										// renderPass
				Renderer::GetShader("Outline"),	// shader
				{													// layout
					{ ShaderDataType::FLOAT3_32, "position" },
					{ ShaderDataType::FLOAT3_32, "normal" },
					{ ShaderDataType::FLOAT3_32, "tangent" },
					{ ShaderDataType::FLOAT3_32, "bitangent" },
					{ ShaderDataType::FLOAT2_32, "texCoord" },
				},
				true,												// testDepth
				false,												// writeDepth
				true,												// hasStencil
				false,												// writeStencil
			};

			mOutlinePipeline = Pipeline::Create(pipelineProperties);
		}

		LoadLanguage("Engine/Localization/english.json");
	}

	void EditorPanel::OnDestroy()
	{
		delete mHierarchyInspector;
		delete mResourcesPanel;

		ResourceManager::Clear();
		
		mScene->Destroy();

		mFramebuffer->Destroy();
		mRenderPass->Destroy();

		mOutlinePipeline->Destroy();
		mGeometryPipeline->Destroy();
		mGridPipeline->Destroy();

		mCommandBuffer->Destroy();
	}

	void EditorPanel::Update(float aDeltaTime)
	{
		mCommandBuffer->BeginCommand();
		mRenderPass->BeginRenderPass(mCommandBuffer, mFramebuffer);

		switch (mEditorMode)
		{
		case EditorMode::EDITOR:
		{
			if (mNeedToUpdateFramebuffer)
			{
				mFramebuffer->Resize(mRenderPass, (uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
				mCamera.Resize(mViewportSize.x, mViewportSize.y);
				mNeedToUpdateFramebuffer = false;
			}

			mCamera.Update(aDeltaTime);

			mScene->UpdatePointLights(mCamera.GetProjectionMatrix(), mCamera.GetViewMatrix(), mCamera.GetPosition());

			DrawGrid();
			DrawGeometry();
			DrawOutline();

			break;
		}
		case EditorMode::PLAY:
		{
			const auto& view{ mScene->GetWorld().view<TransformComponent, CameraComponent>() };
			const auto& cameraTransform{ mScene->GetComponent<TransformComponent>(view.front()) };
			auto& cameraComponent{ mScene->GetComponent<CameraComponent>(view.front()) };
			if (mNeedToUpdateFramebuffer)
			{
				mFramebuffer->Resize(mRenderPass, (uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
				cameraComponent.aspectRatio = mViewportSize.x / mViewportSize.y;
				mNeedToUpdateFramebuffer = false;
			}

			glm::mat4 cameraProjectionMatrix{ glm::perspective(cameraComponent.fov, cameraComponent.aspectRatio, cameraComponent.nearPlane, cameraComponent.farPlane) };
			glm::mat4 cameraViewMatrix{ glm::lookAt(cameraTransform.position, cameraTransform.position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0, 1.0, 0.0)) };

			mScene->UpdatePointLights(cameraProjectionMatrix, cameraViewMatrix, cameraTransform.position);

			DrawGrid();
			DrawGeometry();

			break;
		}
		}

		mRenderPass->EndRenderPass(mCommandBuffer);
		mCommandBuffer->EndCommand();
		mCommandBuffer->Submit();
	}

	void EditorPanel::RenderImGui()
	{
		static bool dockspaceOpen{ true };
		static ImGuiDockNodeFlags dockspaceFlags{ ImGuiDockNodeFlags_None };

		ImGuiWindowFlags windowFlags{ ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking };
		ImGuiViewport* viewport{ ImGui::GetMainViewport() };
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

		ImGuiIO& io{ ImGui::GetIO() };
		ImGuiStyle& style{ ImGui::GetStyle() };
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID id{ ImGui::GetID("Dockspace") };
			ImGui::DockSpace(id, { 0.0f, 0.0f }, dockspaceFlags);
		}

		RenderMainMenuBar();

		RenderViewport();

		mHierarchyInspector->Render();
		mResourcesPanel->Render();

		ImGui::End();
	}
	
	void EditorPanel::OnEvent(Event& aEvent)
	{
		EventDispatcher dispatcher(aEvent);

		dispatcher.Dispatch<KeyPressedEvent>(ILG_BIND_FN(EditorPanel::OnKeyEvent));

		mResourcesPanel->OnEvent(aEvent);
	}

	void EditorPanel::DrawGrid()
	{
		mGridPipeline->Bind(mCommandBuffer);
		mGridPipeline->BindUniformBuffer(mCommandBuffer, mScene->GetSceneDataUBO(), 1);

		Renderer::DrawDefault(mCommandBuffer);
	}

	void EditorPanel::DrawGeometry()
	{
		auto ent{ *mScene->GetWorld().view<TransformComponent, DirectionalLightComponent>().begin() };

		auto [trans, light] { mScene->GetWorld().view<TransformComponent, DirectionalLightComponent>().get<>(ent)};

		const auto& view{ mScene->GetWorld().view<TransformComponent, StaticMeshComponent>() };
		for (auto entity : view)
		{
			auto [transform, meshComponent] { view.get<TransformComponent, StaticMeshComponent>(entity)};

			auto mesh{ meshComponent.staticMesh.lock() };
			auto material{ meshComponent.material.lock() };
			if (!mesh)
				continue;

			mGeometryPipeline->Bind(mCommandBuffer);
			mGeometryPipeline->BindMaterial(mCommandBuffer, material ? material : Renderer::GetDefaultMaterial(), 0);
			mGeometryPipeline->BindUniformBuffer(mCommandBuffer, mScene->GetSceneDataUBO(), 1);
			mGeometryPipeline->PushConstants(mCommandBuffer, 0, 64, glm::value_ptr(transform.worldTransform));
			mGeometryPipeline->PushConstants(mCommandBuffer, 64, 12, glm::value_ptr(light.radiance));
			mGeometryPipeline->PushConstants(mCommandBuffer, 76, 12, glm::value_ptr(glm::radians(trans.rotation)));
			Renderer::SubmitGeometry(mCommandBuffer, mesh);
		}
	}

	void EditorPanel::DrawOutline()
	{
		Entity selectedEntity{ mHierarchyInspector->GetSelected() };
		if (selectedEntity != entt::null && mScene->GetWorld().try_get<StaticMeshComponent>(selectedEntity))
		{
			mOutlinePipeline->Bind(mCommandBuffer);

			const auto&& [transform, meshComponent] { mScene->GetWorld().get<TransformComponent, StaticMeshComponent>(selectedEntity)};
			const auto& mesh{ meshComponent.staticMesh.lock() };

			glm::vec3 position, rotation, scale;
			ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform.worldTransform), glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale));
			mStencilMatrix = glm::translate(glm::mat4(1.0), position) * glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z));
			mStencilMatrix = glm::scale(mStencilMatrix, scale * 1.05f);
			mOutlinePipeline->BindUniformBuffer(mCommandBuffer, mScene->GetSceneDataUBO(), 1);
			mOutlinePipeline->PushConstants(mCommandBuffer, 0, 64, glm::value_ptr(mStencilMatrix));

			Renderer::SubmitGeometry(mCommandBuffer, mesh);
		}
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

			bool enabled{ mHierarchyInspector->GetSelected() != entt::null ? true : false };
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
				mScene->DestroyEntity(mHierarchyInspector->GetSelected());
				mHierarchyInspector->ResetSelected();
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
		ImVec2 frameViewportSize{ ImGui::GetContentRegionAvail() };
		ImVec2 viewportPosition{ ImGui::GetWindowPos() };

		ImGui::Image(mFramebuffer->GetID(), frameViewportSize, { 0.0f, 1.0f }, { 1.0f, 0.0f });

		if (mViewportSize != frameViewportSize)
		{
			mViewportSize = glm::vec2(frameViewportSize.x, frameViewportSize.y);
			mNeedToUpdateFramebuffer = true;
		}

		Entity entity{ mHierarchyInspector->GetSelected() };
		// Guizmo
		if (entity != entt::null)
		{
			ImGuizmo::Enable(true);
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetGizmoSizeClipSpace(0.15f);

			const glm::mat4& viewMatrix{ mCamera.GetViewMatrix() };
			const glm::mat4& projMatrix{ mCamera.GetProjectionMatrix() };

			TransformComponent& transformComp{ mScene->GetWorld().get<TransformComponent>(entity) };
			glm::mat4& transform{ transformComp.localTransform };

			ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projMatrix), (ImGuizmo::OPERATION)mOperation, ImGuizmo::WORLD, glm::value_ptr(transform));

			if (ImGuizmo::IsUsingAny())
			{
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), glm::value_ptr(transformComp.position), glm::value_ptr(transformComp.rotation), glm::value_ptr(transformComp.scale));
				TransformComponent& transformComponent{ mScene->GetComponent<TransformComponent>(entity) };
				if (mScene->HasComponent<ParentComponent>(entity))
				{
					const ParentComponent& parentComponent{ mScene->GetComponent<ParentComponent>(entity) };
					TransformComponent& parentTransformComponent{ mScene->GetComponent<TransformComponent>(parentComponent.parent) };
					transformComponent.CalculateWorldTransform(parentTransformComponent.worldTransform);
				}
				else
				{
					transformComponent.CalculateWorldTransform(glm::mat4(1.0));
				}
				mScene->CalculateChildrenTransforms(entity, transformComponent.worldTransform);
			}
		}

		if (ImGui::BeginDragDropTarget())
		{
			auto payload{ ImGui::AcceptDragDropPayload("MODEL") };

			if (auto payload{ ImGui::AcceptDragDropPayload("MODEL") }; payload)
			{
				UUID uuid{ *(UUID*)payload->Data };
				const ResourceMetadata& metadata{ ResourceManager::GetMetadata(uuid) };

				std::shared_ptr<Model> resource{ std::static_pointer_cast<Model>(ResourceManager::GetResource(uuid)) };

				mScene->LoadModel(resource);
			}
			else if (auto payload{ ImGui::AcceptDragDropPayload("SCENE") }; payload)
			{
				UUID uuid{ *(UUID*)payload->Data };
				const ResourceMetadata& metadata{ ResourceManager::GetMetadata(uuid) };
				std::shared_ptr<Scene> resource{ std::static_pointer_cast<Scene>(ResourceManager::GetResource(uuid)) };
				
				mScene->Destroy();
				mScene = resource;
				mHierarchyInspector->SetScene(mScene);
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::SetNextWindowPos({ frameViewportSize.x * 0.5f - 100.0f, viewportPosition.y + 5.0f });
		ImGui::BeginChild("Play/Stop", { 200.0f, 25.0f }, true, ImGuiWindowFlags_NoDecoration);
		if (ImGui::Button("Play", { 50.0f, 20.0f }))
		{
			SaveScene("Resources/Demo.ilargi");
			mEditorMode = EditorMode::PLAY;
			const auto& view{ mScene->GetWorld().view<TransformComponent, CameraComponent>() };
			const auto& cameraTransform{ mScene->GetComponent<TransformComponent>(view.front()) };
			auto& cameraComponent{ mScene->GetComponent<CameraComponent>(view.front()) };
			cameraComponent.aspectRatio = mViewportSize.x / mViewportSize.y;
		}
		ImGui::SameLine();
		if (ImGui::Button("Pause", { 50.0f, 20.0f }))
		{
			mEditorMode = EditorMode::PAUSE;
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop", { 50.0f, 20.0f }))
		{
			OpenScene(mScene->mResourceUUID);
			mEditorMode = EditorMode::EDITOR;
		}
		ImGui::EndChild();
	}

	void EditorPanel::NewScene()
	{
		mScene = std::make_shared<Scene>();
		mHierarchyInspector->SetScene(mScene);

		Entity entity{ mScene->CreateEntity("Directional Light") };
		mScene->CreateComponent<DirectionalLightComponent>(entity);

		Entity cameraEntity{ mScene->CreateEntity("Main Camera") };
		mScene->CreateComponent<CameraComponent>(cameraEntity);
	}

	void EditorPanel::OpenScene()
	{
		//std::string filepath = FileSystem::OpenFile("Ilargi Scene (.ilargi)\0*.ilargi\0");
		//if (!filepath.empty())
		//	OpenScene(filepath);
	}

	void EditorPanel::OpenScene(UUID aUUID)
	{
		mScene = std::static_pointer_cast<Scene>(ResourceManager::ReloadResource(aUUID));
	}

	void EditorPanel::SaveSceneAs()
	{
		std::string filepath{ FileSystem::SaveFile("Ilargi Scene (.ilargi)\0*.ilargi\0") };
		if (!filepath.empty())
		{
			SaveScene(filepath);
			mResourcesPanel->RefreshAssets();
		}
	}

	void EditorPanel::SaveScene(std::string aFilepath)
	{
		SceneImporter::SaveScene(mScene, aFilepath);

		// TODO: Think a better way of handle this if possible
		auto start{ aFilepath.find("Resources") };
		mScene->mResourceUUID = ResourceManager::ImportResource(std::filesystem::path(aFilepath.substr(start)).remove_filename(), std::filesystem::path(aFilepath.substr(start)));
	}
	
	bool EditorPanel::OnKeyEvent(KeyPressedEvent& aEvent)
	{
		bool ctrl{ Input::IsKeyPressed(KeyCode::LEFT_CONTROL) || Input::IsKeyPressed(KeyCode::RIGHT_CONTROL) };
		bool shift{ Input::IsKeyPressed(KeyCode::LEFT_SHIFT) || Input::IsKeyPressed(KeyCode::RIGHT_SHIFT) };
		bool alt{ Input::IsKeyPressed(KeyCode::LEFT_ALT) || Input::IsKeyPressed(KeyCode::RIGHT_ALT) };

		switch (aEvent.GetKey())
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
				mOperation = ImGuizmo::TRANSLATE;
			break;
		case KeyCode::E:
			if (!Input::IsMouseButtonPressed(MouseCode::RIGHT))
				mOperation = ImGuizmo::ROTATE;
			break;
		case KeyCode::R:
			if (!Input::IsMouseButtonPressed(MouseCode::RIGHT))
				mOperation = ImGuizmo::SCALE;
			break;
		case KeyCode::F4:
			Application::Get()->CloseApp();
			break;
		}

		return true;
	}
}