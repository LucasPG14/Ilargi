#include "ilargipch.h"

#include "EditorPanel.h"
#include "EditorPanels/SceneHierarchyInspectorPanel.h"
#include "EditorPanels/ResourcesPanel.h"

#include "Resources/Mesh.h"

#include "Utils/Importers/ModelImporter.h"

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <gtc/type_ptr.hpp>

namespace Ilargi
{
	EditorPanel::EditorPanel() : Panel("Editor Panel"), hierarchyInspector(nullptr), resourcesPanel(nullptr), 
		viewportSize({ 1080, 720 }), needToUpdateFramebuffer(false)
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
		
		framebuffer = Framebuffer::Create({ 1080, 720, {ImageFormat::RGBA8}, false });	
		{
			PipelineProperties pipelineProperties;
			pipelineProperties.name = "Geometry";
			pipelineProperties.shader = Shader::Create("shaders/shaderfull.vert");
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
		
		uboCamera = UniformBuffer::Create(sizeof(glm::mat4), Renderer::GetConfig().maxFrames);

	}

	void EditorPanel::OnDestroy()
	{
		uboCamera->Destroy();
		
		scene->Destroy();

		framebuffer->Destroy();
		renderPass->Destroy();
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

			const glm::mat4& rotationMat = glm::toMat4(glm::quat(glm::radians(transform.rotation)));

			transform.CalculateTransform();
			
			renderPass->GetProperties().pipeline->Bind(commandBuffer);
			renderPass->GetProperties().pipeline->BindDescriptorSet(commandBuffer, mesh.staticMesh->GetMaterial());
			renderPass->GetProperties().pipeline->PushConstants(commandBuffer, 0, 64, glm::value_ptr(transform.transform));
			renderPass->GetProperties().pipeline->PushConstants(commandBuffer, 64, 64, glm::value_ptr(constants[0]));
			renderPass->GetProperties().pipeline->PushConstants(commandBuffer, 128, 16, glm::value_ptr(mesh.staticMesh->GetColor()));
			Renderer::SubmitGeometry(commandBuffer, mesh.staticMesh);
		}

		renderPass->EndRenderPass(commandBuffer);
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

		//ImGui::BeginMainMenuBar();
		//if (ImGui::BeginMenu("File"))
		//{
		//	if (ImGui::MenuItem("New Scene", "Ctrl + N"))
		//	{

		//	}
		//	if (ImGui::MenuItem("Open Scene", "Ctrl + O"))
		//	{

		//	}
		//	ImGui::EndMenu();
		//}
		//ImGui::EndMainMenuBar();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::Begin("Viewport");
		ImVec2 frameViewportSize = ImGui::GetContentRegionAvail();
		
		ImGui::Image(framebuffer->GetID(), frameViewportSize, { 0.0f, 1.0f }, { 1.0f, 0.0f });

		if (frameViewportSize.x != viewportSize.x || frameViewportSize.y != viewportSize.y)
		{
			viewportSize = { frameViewportSize.x, frameViewportSize.y };
			needToUpdateFramebuffer = true;
		}

		ImGui::End();
		ImGui::PopStyleVar();

		hierarchyInspector->Render();
		resourcesPanel->Render();

		ImGui::End();
	}
	
	void EditorPanel::OnEvent(Event& event)
	{
	}
}