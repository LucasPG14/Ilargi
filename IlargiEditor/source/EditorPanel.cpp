#include "ilargipch.h"

#include "EditorPanel.h"
#include "EditorPanels/SceneHierarchyInspectorPanel.h"
#include "EditorPanels/ResourcesPanel.h"

#include "LocalizationManager.h"

#include "Resources/Model.h"
#include "Utils/FileSystem.h"
#include "Utils/Importers/SceneImporter.h"

#include "Renderer/PipelineManager.h"

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
		
		mMousePickingFramebuffer = Framebuffer::Create({ { ImageFormat::RED32_UINT }, 1080U, 720U, false, false });

		mFramebuffer = Framebuffer::Create({ { ImageFormat::RGBA8, ImageFormat::DEPTH24_STENCIL8 }, 1080U, 720U, false, true });

		//{
		//	PipelineProperties pipelineProperties
		//	{
		//		"Outline",											// name
		//		mRenderPass,										// renderPass
		//		Renderer::GetShader("Outline"),	// shader
		//		{													// layout
		//			{ ShaderDataType::FLOAT3_32, "position" },
		//			{ ShaderDataType::FLOAT3_32, "normal" },
		//			{ ShaderDataType::FLOAT3_32, "tangent" },
		//			{ ShaderDataType::FLOAT3_32, "bitangent" },
		//			{ ShaderDataType::FLOAT2_32, "texCoord" },
		//		},
		//		1U,
		//		true,												// testDepth
		//		false,												// writeDepth
		//		true,												// hasStencil
		//		false,												// writeStencil
		//	};

		//	mOutlinePipeline = Pipeline::Create(pipelineProperties);
		//}

		//{
		//	PipelineProperties pipelineProperties
		//	{
		//		"MousePicking",										// name
		//		mMousePickingRenderPass,							// renderPass
		//		Renderer::GetShader("MousePicking"),				// shader
		//		{													// layout
		//			{ ShaderDataType::FLOAT3_32, "position" },
		//			{ ShaderDataType::FLOAT3_32, "normal" },
		//			{ ShaderDataType::FLOAT3_32, "tangent" },
		//			{ ShaderDataType::FLOAT3_32, "bitangent" },
		//			{ ShaderDataType::FLOAT2_32, "texCoord" },
		//		},
		//		1U,
		//		true,												// testDepth
		//		false,												// writeDepth
		//		true,												// hasStencil
		//		false,												// writeStencil
		//		false,												// blend
		//	};

		//	mMousePickingPipeline = Pipeline::Create(pipelineProperties);
		//}

		LocalizationManager::LoadLanguage("Engine/Localization/english.json");
	}

	void EditorPanel::OnDestroy()
	{
		delete mHierarchyInspector;
		delete mResourcesPanel;
		
		mScene->Destroy();

		mFramebuffer->Destroy();
		mMousePickingFramebuffer->Destroy();

		//mOutlinePipeline->Destroy();

		//mMousePickingPipeline->Destroy();

		mCommandBuffer->Destroy();
	}

	void EditorPanel::Update(float aDeltaTime)
	{
		mCommandBuffer->BeginCommand();

		switch (mEditorMode)
		{
		case EditorMode::EDITOR:
		{
			if (mNeedToUpdateFramebuffer)
			{
				mFramebuffer->Resize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
				mMousePickingFramebuffer->Resize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
				mCamera.Resize(mViewportSize.x, mViewportSize.y);
				mNeedToUpdateFramebuffer = false;
			}

			const auto& RenderPass{ Renderer::GetRenderPass({ mFramebuffer->GetProperties().Formats, true }) };
			RenderPass->BeginRenderPass(mCommandBuffer, mFramebuffer);

			mCamera.Update(aDeltaTime);

			mScene->Update();

			mScene->UpdatePointLights(mCamera.GetProjectionMatrix(), mCamera.GetViewMatrix(), mCamera.GetPosition());

			DrawGrid();
			DrawGeometry();
			//DrawOutline();

			RenderPass->EndRenderPass(mCommandBuffer);

			//mRenderPass->EndRenderPass(mCommandBuffer);

			//const auto& trView{ mScene->GetWorld().view<TransformComponent, StaticMeshComponent>() };
			//if (Input::IsMouseButtonPressed(MouseCode::LEFT) && trView.begin() != trView.end())
			//{
			//	glm::vec2 mousePos{ Input::GetMousePos() };
			//	float mouseX { mousePos.x - mViewportPosition.x };
			//	float mouseY { mousePos.y - mViewportPosition.y };
			//	if (mouseX > 0 && mouseY > 0 && mouseX <= mViewportSize.x && mouseY <= mViewportSize.y)
			//	{
			//		mouseX = mouseX / mViewportSize.x;
			//		mouseY = mouseY / mViewportSize.y;
			//		mMousePickingRenderPass->BeginRenderPass(mCommandBuffer, mMousePickingFramebuffer);
			//		mMousePickingPipeline->Bind(mCommandBuffer);

			//		ShaderStage stage{ ShaderStage(3) };
			//		for (auto entity : trView)
			//		{
			//			auto [transform, meshComponent] { trView.get<TransformComponent, StaticMeshComponent>(entity)};

			//			for (uint32_t index{ 0U }; index < meshComponent.submeshes.size(); ++index)
			//			{
			//				mMousePickingPipeline->BindUniformBuffer(mCommandBuffer, mScene->GetSceneDataUBO(), 0);
			//				mMousePickingPipeline->PushConstants(mCommandBuffer, stage, 0, 64, glm::value_ptr(transform.worldTransform));
			//				mMousePickingPipeline->PushConstants(mCommandBuffer, stage, 64, 4, &entity);
			//				Renderer::SubmitGeometry(mCommandBuffer, std::static_pointer_cast<StaticMesh>(ResourceManager::GetResource(meshComponent.submeshes[index].mesh)));
			//			}
			//		}

			//		mMousePickingRenderPass->EndRenderPass(mCommandBuffer);

			//		uint32_t objectID{ mMousePickingFramebuffer->ReadFramebufferPixel(mouseX, mouseY) };

			//		mHierarchyInspector->SetSelected(objectID);
			//	}
			//}
			break;
		}
		case EditorMode::PLAY:
		{
			const auto& view{ mScene->GetWorld().view<TransformComponent, CameraComponent>() };
			const auto& cameraTransform{ mScene->GetComponent<TransformComponent>(view.front()) };
			auto& cameraComponent{ mScene->GetComponent<CameraComponent>(view.front()) };
			if (mNeedToUpdateFramebuffer)
			{
				mFramebuffer->Resize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
				mMousePickingFramebuffer->Resize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
				cameraComponent.aspectRatio = mViewportSize.x / mViewportSize.y;
				mNeedToUpdateFramebuffer = false;
			}

			glm::mat4 cameraProjectionMatrix{ glm::perspective(cameraComponent.fov, cameraComponent.aspectRatio, cameraComponent.nearPlane, cameraComponent.farPlane) };
			glm::mat4 cameraViewMatrix{ glm::lookAt(cameraTransform.position, cameraTransform.position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0, 1.0, 0.0)) };

			mScene->UpdatePointLights(cameraProjectionMatrix, cameraViewMatrix, cameraTransform.position);

			DrawGrid();
			//DrawGeometry();

			break;
		}
		}

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
		DepthState depthState;
		depthState.Enabled = true;
		depthState.Test = true;
		depthState.Write = false;
		depthState.CompareOp = CompareOp::LESS;

		const auto& Pipeline{ Renderer::GetPipeline({"Grid", {}, { ImageFormat::RGBA8, ImageFormat::DEPTH24_STENCIL8 }, {}, depthState, {}, 1}) };
		Pipeline->Bind(mCommandBuffer);
		Pipeline->BindUniformBuffer(mCommandBuffer, mScene->GetSceneDataUBO(), 0);

		Renderer::DrawDefault(mCommandBuffer);
	}

	void EditorPanel::DrawGeometry()
	{
		DepthState depthState {true, true, true, CompareOp::LESS};
		depthState.StencilState.Enabled = true;
		depthState.StencilState.Back = { CompareOp::ALWAYS, StencilOp::REPLACE, StencilOp::KEEP, StencilOp::KEEP, 0xFFU, 0xFFU, 1U };
		depthState.StencilState.Front = { CompareOp::ALWAYS, StencilOp::REPLACE, StencilOp::KEEP, StencilOp::KEEP, 0xFFU, 0xFFU, 1U };

		RasterState rasterState { CullMode::NONE, FillMode::FILL, FrontFace::COUNTER_CLOCKWISE, false, false };

		const std::shared_ptr<Pipeline>& Pipeline{ Renderer::GetPipeline({"PBR_Static", {{ ShaderDataType::FLOAT3_32, "position" },
						{ ShaderDataType::FLOAT3_32, "normal" },
						{ ShaderDataType::FLOAT3_32, "tangent" },
						{ ShaderDataType::FLOAT3_32, "bitangent" },
						{ ShaderDataType::FLOAT2_32, "texCoord" }}, { ImageFormat::RGBA8, ImageFormat::DEPTH24_STENCIL8 }, rasterState, depthState, {}, 1}) };
		
		Pipeline->Bind(mCommandBuffer);
		auto ent{ *mScene->GetWorld().view<TransformComponent, DirectionalLightComponent>().begin() };

		auto [trans, light] { mScene->GetWorld().view<TransformComponent, DirectionalLightComponent>().get<>(ent)};

		const auto& view{ mScene->GetWorld().view<TransformComponent, StaticMeshComponent>() };
		for (auto entity : view)
		{
			auto [transform, meshComponent] { view.get<TransformComponent, StaticMeshComponent>(entity)};

			for (uint32_t index{ 0U }; index < meshComponent.submeshes.size(); ++index)
			{
				Pipeline->BindMaterial(mCommandBuffer, std::static_pointer_cast<Material>(ResourceManager::GetResource(meshComponent.submeshes[index].material)), 1);
				Pipeline->BindUniformBuffer(mCommandBuffer, mScene->GetSceneDataUBO(), 0);
				Pipeline->PushConstants(mCommandBuffer, VERTEX_SHADER, 0, 64, glm::value_ptr(transform.worldTransform));
				Pipeline->PushConstants(mCommandBuffer, VERTEX_SHADER, 64, 12, glm::value_ptr(light.radiance));
				Pipeline->PushConstants(mCommandBuffer, VERTEX_SHADER, 76, 12, glm::value_ptr(trans.rotation));
				Renderer::SubmitGeometry(mCommandBuffer, std::static_pointer_cast<StaticMesh>(ResourceManager::GetResource(meshComponent.submeshes[index].mesh)));
			}
		}
	}

	void EditorPanel::DrawOutline()
	{
		// TODO: MESHES
		Entity selectedEntity{ mHierarchyInspector->GetSelected() };
		if (selectedEntity != entt::null && mScene->GetWorld().try_get<StaticMeshComponent>(selectedEntity))
		{
			mOutlinePipeline->Bind(mCommandBuffer);

			const auto&& [transform, meshComponent] { mScene->GetWorld().get<TransformComponent, StaticMeshComponent>(selectedEntity)};
			
			glm::vec3 position, rotation, scale;
			ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform.worldTransform), glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale));
			mStencilMatrix = glm::translate(glm::mat4(1.0), position) * glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z));
			mStencilMatrix = glm::scale(mStencilMatrix, scale * 1.05f);

			mOutlinePipeline->BindUniformBuffer(mCommandBuffer, mScene->GetSceneDataUBO(), 0);
			mOutlinePipeline->PushConstants(mCommandBuffer, VERTEX_SHADER, 0, 64, glm::value_ptr(mStencilMatrix));

			for (const auto& submesh : meshComponent.submeshes)
			{
				const auto& mesh{ std::static_pointer_cast<StaticMesh>(ResourceManager::GetResource(submesh.mesh)) };

				Renderer::SubmitGeometry(mCommandBuffer, mesh);
			}
		}
	}

	void EditorPanel::RenderMainMenuBar()
	{
		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu(LOC("editor.file")))
		{
			if (ImGui::MenuItem(LOC("editor.file.newscene"), "Ctrl + N"))
			{
				NewScene();
			}
			if (ImGui::MenuItem(LOC("editor.file.openscene"), "Ctrl + O"))
			{
				OpenScene();
			}
			ImGui::Separator();
			if (ImGui::MenuItem(LOC("editor.file.savescene"), "Ctrl + S"))
			{
				// TODO: Change this to save the scene with the current path of the scene
				SaveSceneAs();
			}
			if (ImGui::MenuItem(LOC("editor.file.savesceneas"), "Ctrl + Shift + S"))
			{
				SaveSceneAs();
			}
			ImGui::Separator();
			if (ImGui::MenuItem(LOC("editor.file.exit"), "Ctrl + Alt + F4"))
			{
				Application::Get()->CloseApp();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu(LOC("editor.edit")))
		{
			if (ImGui::MenuItem(LOC("editor.edit.undo"), "Ctrl + Z"))
			{
				// TODO: Undo
			}
			if (ImGui::MenuItem(LOC("editor.edit.redo"), "Ctrl + Y"))
			{
				// TODO: Redo
			}
			ImGui::Separator();

			bool enabled{ mHierarchyInspector->GetSelected() != entt::null ? true : false };
			if (ImGui::MenuItem(LOC("editor.edit.copy"), "Ctrl + C", (bool*)0, enabled))
			{
				// TODO: Copy
			}
			if (ImGui::MenuItem(LOC("editor.edit.paste"), "Ctrl + V", (bool*)0, enabled))
			{
				// TODO: Paste
			}
			if (ImGui::MenuItem(LOC("editor.edit.delete"), "Del", (bool*)0, enabled))
			{
				mScene->DestroyEntity(mHierarchyInspector->GetSelected());
				mHierarchyInspector->ResetSelected();
			}
			if (ImGui::MenuItem(LOC("editor.edit.duplicate"), "Ctrl + D", (bool*)0, enabled))
			{
				// TODO: Duplicate an entity
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu(LOC("editor.localization")))
		{
			if (ImGui::MenuItem(LOC("editor.localization.english")))
			{
				LocalizationManager::LoadLanguage("Engine/Localization/english.json");
			}
			if (ImGui::MenuItem(LOC("editor.localization.spanish")))
			{
				LocalizationManager::LoadLanguage("Engine/Localization/spanish.json");
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
		mViewportPosition = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };

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

		ImGui::SetNextWindowPos({ frameViewportSize.x * 0.5f - 100.0f, mViewportPosition.y + 5.0f });
		ImGui::BeginChild("Play/Stop", { 200.0f, 25.0f }, true, ImGuiWindowFlags_NoDecoration);
		if (ImGui::Button(LOC("editor.viewport.play"), { 50.0f, 20.0f }))
		{
			SaveScene("Resources/Demo.ilargi");
			mEditorMode = EditorMode::PLAY;
			const auto& view{ mScene->GetWorld().view<TransformComponent, CameraComponent>() };
			const auto& cameraTransform{ mScene->GetComponent<TransformComponent>(view.front()) };
			auto& cameraComponent{ mScene->GetComponent<CameraComponent>(view.front()) };
			cameraComponent.aspectRatio = mViewportSize.x / mViewportSize.y;
		}
		ImGui::SameLine();
		if (ImGui::Button(LOC("editor.viewport.pause"), { 50.0f, 20.0f }))
		{
			mEditorMode = EditorMode::PAUSE;
		}
		ImGui::SameLine();
		if (ImGui::Button(LOC("editor.viewport.stop"), { 50.0f, 20.0f }))
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
		mScene->GetComponent<TransformComponent>(entity).rotation = { 0.0f, 80.0f, 45.0f };
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