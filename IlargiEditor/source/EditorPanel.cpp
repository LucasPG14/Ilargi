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

		mAppIcon = Texture2D::Create("Engine/Textures/Icon.png");
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
		
		mAppIcon.reset();
		mScene->Destroy();

		mFramebuffer->Destroy();
		mMousePickingFramebuffer->Destroy();

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

			const auto& RenderPass{ Renderer::GetRenderPass({mFramebuffer->GetProperties().Formats}) };
			RenderPass->BeginRenderPass(mCommandBuffer, mFramebuffer);

			mCamera.Update(aDeltaTime);

			mScene->Update();

			mScene->UpdatePointLights(mCamera.GetProjectionMatrix(), mCamera.GetViewMatrix(), mCamera.GetPosition());

			DrawGrid();
			DrawGeometry();
			DrawOutline();

			RenderPass->EndRenderPass(mCommandBuffer);

			const auto& trView{ mScene->GetWorld().view<TransformComponent, StaticMeshComponent>() };
			if (Input::IsMouseButtonPressed(MouseCode::LEFT) && trView.begin() != trView.end())
			{
				glm::vec2 mousePos{ Input::GetMousePos() };
				int mouseX { static_cast<int>(mousePos.x - mViewportPosition.x) };
				int mouseY { static_cast<int>(mousePos.y - mViewportPosition.y) };
				if (mouseX > 0 && mouseY > 0 && mouseX <= mViewportSize.x && mouseY <= mViewportSize.y)
				{					
					const auto& MousePickingRenderPass{ Renderer::GetRenderPass({ mMousePickingFramebuffer->GetProperties().Formats }) };
					MousePickingRenderPass->BeginRenderPass(mCommandBuffer, mMousePickingFramebuffer);
					
					BlendState blendState;
					blendState.ColorMask = ColorMask::NONE;
					blendState.Enabled = false;

					DepthState depthState;
					depthState.Write = false;
					depthState.StencilState.Enabled = false;

					const auto& Pipeline{ Renderer::GetPipeline({"MousePicking", {{ ShaderDataType::FLOAT3_32, "position" },
									{ ShaderDataType::FLOAT3_32, "normal" },
									{ ShaderDataType::FLOAT4_32, "tangent" },
									{ ShaderDataType::FLOAT2_32, "texCoord" },}, { ImageFormat::RED32_UINT }, {}, {}, blendState, 1}) };
					
					Pipeline->Bind(mCommandBuffer);

					ShaderStage stage{ ShaderStage(3) };
					for (const auto& entity : trView)
					{
						auto [transform, meshComponent] { trView.get<TransformComponent, StaticMeshComponent>(entity)};

						for (uint32_t index{ 0U }; index < meshComponent.submeshes.size(); ++index)
						{
							Pipeline->BindUniformBuffer(mCommandBuffer, mScene->GetSceneDataUBO(), 0);
							Pipeline->PushConstants(mCommandBuffer, stage, 0, 64, glm::value_ptr(transform.worldTransform));
							Pipeline->PushConstants(mCommandBuffer, stage, 64, 4, &entity);
							Renderer::SubmitGeometry(mCommandBuffer, std::static_pointer_cast<StaticMesh>(ResourceManager::GetResource(meshComponent.submeshes[index].mesh)));
						}
					}

					MousePickingRenderPass->EndRenderPass(mCommandBuffer);

					uint32_t objectID{ mMousePickingFramebuffer->ReadFramebufferPixel(mouseX, mouseY) };

					mHierarchyInspector->SetSelected(objectID);
				}
			}
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
		ImGuiViewport* vp = ImGui::GetMainViewport();

		ImGui::SetNextWindowPos(vp->Pos);
		ImGui::SetNextWindowSize(vp->Size);
		ImGui::SetNextWindowViewport(vp->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("RootWindow", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
		ImGui::PopStyleVar();

		RenderMainMenuBar();

		ImGui::EndGroup();

		ImGui::SetCursorPosY(64.0f);

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");

		ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

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
		depthState.StencilState.Enabled = false;

		RasterState rasterState { CullMode::NONE, FillMode::FILL, FrontFace::COUNTER_CLOCKWISE, false, false };

		const std::shared_ptr<Pipeline>& Pipeline{ Renderer::GetPipeline({"PBR_Static", {
			{ ShaderDataType::FLOAT3_32, "position" },
			{ ShaderDataType::FLOAT3_32, "normal" },
			{ ShaderDataType::FLOAT4_32, "tangent" },
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
		Entity selectedEntity{ mHierarchyInspector->GetSelected() };
		if (selectedEntity != entt::null && mScene->GetWorld().try_get<StaticMeshComponent>(selectedEntity))
		{
			// Write stencil
			{
				BlendState blendState;
				blendState.Enabled = false;
				blendState.ColorMask = ColorMask::NONE;

				DepthState writeStencilDepthState{ true, false, true, CompareOp::LESS };
				writeStencilDepthState.StencilState.Enabled = true;
				writeStencilDepthState.StencilState.Back = { CompareOp::ALWAYS, StencilOp::REPLACE, StencilOp::KEEP, StencilOp::KEEP, 0xFFU, 0xFFU, 1U };
				writeStencilDepthState.StencilState.Front = { CompareOp::ALWAYS, StencilOp::REPLACE, StencilOp::KEEP, StencilOp::KEEP, 0xFFU, 0xFFU, 1U };

				RasterState rasterState{ CullMode::NONE, FillMode::FILL, FrontFace::COUNTER_CLOCKWISE, false, false };

				const std::shared_ptr<Pipeline>& Pipeline{ Renderer::GetPipeline({"Outline", {
					{ ShaderDataType::FLOAT3_32, "position" },
					{ ShaderDataType::FLOAT3_32, "normal" },
					{ ShaderDataType::FLOAT4_32, "tangent" },
					{ ShaderDataType::FLOAT2_32, "texCoord" }}, { ImageFormat::RGBA8, ImageFormat::DEPTH24_STENCIL8 }, rasterState, writeStencilDepthState, blendState, 1}) };



				Pipeline->Bind(mCommandBuffer);

				const auto&& [transform, meshComponent] { mScene->GetWorld().get<TransformComponent, StaticMeshComponent>(selectedEntity)};

				Pipeline->BindUniformBuffer(mCommandBuffer, mScene->GetSceneDataUBO(), 0);
				Pipeline->PushConstants(mCommandBuffer, VERTEX_SHADER, 0, 64, glm::value_ptr(transform.worldTransform));

				for (const auto& submesh : meshComponent.submeshes)
				{
					const auto& mesh{ std::static_pointer_cast<StaticMesh>(ResourceManager::GetResource(submesh.mesh)) };

					Renderer::SubmitGeometry(mCommandBuffer, mesh);
				}
			}

			// Read Stencil
			{
				DepthState readStencilDepthState{ true, false, false, CompareOp::LESS_EQUAL };
				readStencilDepthState.StencilState.Enabled = true;
				readStencilDepthState.StencilState.Back = { CompareOp::NOT_EQUAL, StencilOp::KEEP, StencilOp::KEEP, StencilOp::KEEP, 0xFFU, 0x00U, 1U };
				readStencilDepthState.StencilState.Front = { CompareOp::NOT_EQUAL, StencilOp::KEEP, StencilOp::KEEP, StencilOp::KEEP, 0xFFU, 0x00U, 1U };

				RasterState rasterState{ CullMode::NONE, FillMode::FILL, FrontFace::COUNTER_CLOCKWISE, false, false };

				const std::shared_ptr<Pipeline>& Pipeline{ Renderer::GetPipeline({"Outline", {
					{ ShaderDataType::FLOAT3_32, "position" },
					{ ShaderDataType::FLOAT3_32, "normal" },
					{ ShaderDataType::FLOAT4_32, "tangent" },
					{ ShaderDataType::FLOAT2_32, "texCoord" }}, { ImageFormat::RGBA8, ImageFormat::DEPTH24_STENCIL8 }, rasterState, readStencilDepthState, {}, 1}) };

				Pipeline->Bind(mCommandBuffer);

				const auto&& [transform, meshComponent] { mScene->GetWorld().get<TransformComponent, StaticMeshComponent>(selectedEntity)};

				glm::vec3 position, rotation, scale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform.worldTransform), glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale));
				mStencilMatrix = glm::translate(glm::mat4(1.0), position) * glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z));
				mStencilMatrix = glm::scale(mStencilMatrix, scale * 1.03f);

				Pipeline->BindUniformBuffer(mCommandBuffer, mScene->GetSceneDataUBO(), 0);
				Pipeline->PushConstants(mCommandBuffer, VERTEX_SHADER, 0, 64, glm::value_ptr(mStencilMatrix));

				for (const auto& submesh : meshComponent.submeshes)
				{
					const auto& mesh{ std::static_pointer_cast<StaticMesh>(ResourceManager::GetResource(submesh.mesh)) };

					Renderer::SubmitGeometry(mCommandBuffer, mesh);
				}
			}
		}
	}

	void EditorPanel::RenderMainMenuBar()
	{
		static bool dragging{ false };
		static ImVec2 dragStartMousePos{0.0f, 0.0f};
		ImGui::BeginGroup();

		ImGui::Image((ImTextureID)mAppIcon->GetID(), { 64.0f, 64.0f });

		ImGui::SameLine();

		if (ImGui::IsWindowHovered())
		{
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				Application::Get()->GetWindow().MaximizeWindow();
			}
			else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				dragging = true;

				dragStartMousePos = ImGui::GetMousePos();
			}
		}

		if (dragging)
		{
			if (ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f))
			{
				ImVec2 mousePos{ ImGui::GetMousePos() };

				int newX = mousePos.x - dragStartMousePos.x;
				int newY = mousePos.y - dragStartMousePos.y;

				Application::Get()->GetWindow().SetWindowPosition(newX, newY);
			}
			else
			{
				dragging = false;
			}
		}

		float posWindow{ ImGui::GetCursorPosX() - ImGui::GetStyle().ItemInnerSpacing.x };
		ImGui::AlignTextToFramePadding();

		ImGui::PushStyleColor(ImGuiCol_Header, { 0.10f, 0.10f, 0.11f, 1.00f });

		if (ImGui::Selectable(LOC("editor.file"), true, 0, ImVec2(ImGui::CalcTextSize(LOC("editor.file")).x, 20.0f)))
			ImGui::OpenPopup("FilePopup");

		ImGui::SetNextWindowPos({ posWindow, 25.0f });
		if (ImGui::BeginPopup("FilePopup"))
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
			if (ImGui::MenuItem(LOC("editor.file.settings")))
			{
				// Open Settings Window.
			}
			ImGui::Separator();
			if (ImGui::MenuItem(LOC("editor.file.exit"), "Ctrl + Alt + F4"))
			{
				Application::Get()->CloseApp();
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();

		posWindow = ImGui::GetCursorPosX() - ImGui::GetStyle().ItemInnerSpacing.x;
		ImGui::AlignTextToFramePadding();
		if (ImGui::Selectable(LOC("editor.edit"), true, 0, ImVec2(ImGui::CalcTextSize(LOC("editor.edit")).x, 20.0f)))
			ImGui::OpenPopup("EditPopup");

		ImGui::SetNextWindowPos({ posWindow, 25.0f });
		if (ImGui::BeginPopup("EditPopup"))
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
			ImGui::EndPopup();
		}

		ImGui::SameLine();

		posWindow = ImGui::GetCursorPosX() - ImGui::GetStyle().ItemInnerSpacing.x;
		ImGui::AlignTextToFramePadding();
		if (ImGui::Selectable(LOC("editor.window"), true, 0, ImVec2(ImGui::CalcTextSize(LOC("editor.window")).x, 20.0f)))
			ImGui::OpenPopup("WindowPopup");

		ImGui::SetNextWindowPos({ posWindow, 25.0f });
		if (ImGui::BeginPopup("WindowPopup"))
		{
			//ImGui::Separator();
			//if (ImGui::MenuItem(LOC("editor.localization.english")))
			//{
			//	LocalizationManager::LoadLanguage("Engine/Localization/english.json");
			//}
			//if (ImGui::MenuItem(LOC("editor.localization.spanish")))
			//{
			//	LocalizationManager::LoadLanguage("Engine/Localization/spanish.json");
			//}
			ImGui::EndPopup();
		}

		ImGui::SameLine();

		posWindow = ImGui::GetCursorPosX() - ImGui::GetStyle().ItemInnerSpacing.x;
		ImGui::AlignTextToFramePadding();
		if (ImGui::Selectable(LOC("editor.help"), true, 0, ImVec2(ImGui::CalcTextSize(LOC("editor.help")).x, 20.0f)))
			ImGui::OpenPopup("HelpPopup");

		ImGui::SetNextWindowPos({ posWindow, 25.0f });
		if (ImGui::BeginPopup("HelpPopup"))
		{
			if (ImGui::MenuItem(LOC("editor.help.reportbug")))
			{
				FileSystem::OpenWeb(L"https://github.com/LucasPG14/Ilargi/issues");
			}
			//ImGui::Text("APPLICATION");
			if (ImGui::MenuItem(LOC("editor.help.about")))
			{
				
			}
			ImGui::EndPopup();
		}

		ImGui::PopStyleColor();

		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.10f, 0.10f, 0.11f, 1.00f });
		ImGui::SameLine(ImGui::GetWindowWidth() - 150);
		if (ImGui::Button("-", { 50.0f, 28.0f }))
		{
			Application::Get()->GetWindow().MinimizeWindow();
		}
		ImGui::SameLine(ImGui::GetWindowWidth() - 100);
		if (ImGui::Button("Y", { 50.0f, 28.0f }))
		{
			Application::Get()->GetWindow().MaximizeWindow();
		}
		ImGui::SameLine(ImGui::GetWindowWidth() - 50);
		if (ImGui::Button("X", { 50.0f, 28.0f }))
		{
			Application::Get()->CloseApp();
		}
		ImGui::PopStyleVar(4);
		ImGui::PopStyleColor();



		//
		ImGui::SetCursorPos({ 200.0f, 28.0f });
		//ImGui::Separator();
		if (ImGui::Button("<", { 28.0f, 28.0f }))
		{
			SaveScene("Resources/Demo.ilargi");
			mEditorMode = EditorMode::PLAY;
			const auto& view{ mScene->GetWorld().view<TransformComponent, CameraComponent>() };
			const auto& cameraTransform{ mScene->GetComponent<TransformComponent>(view.front()) };
			auto& cameraComponent{ mScene->GetComponent<CameraComponent>(view.front()) };
			cameraComponent.aspectRatio = mViewportSize.x / mViewportSize.y;
		}
		ImGui::SameLine();
		if (ImGui::Button("||", { 28.0f, 28.0f }))
		{
			mEditorMode = EditorMode::PAUSE;
		}
		ImGui::SameLine();
		if (ImGui::Button("[]", { 28.0f, 28.0f }))
		{
			OpenScene(mScene->mResourceUUID);
			mEditorMode = EditorMode::EDITOR;
		}
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