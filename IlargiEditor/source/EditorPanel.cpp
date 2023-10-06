#include "ilargipch.h"

#include "EditorPanel.h"
#include "EditorPanels/SceneHierarchyInspectorPanel.h"
#include "EditorPanels/ResourcesPanel.h"

#include "Utils/Importers/TinyObjImporter.h"

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <gtc/type_ptr.hpp>

namespace Ilargi
{
	const std::vector<Vertex> vertices = 
	{
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};

	const std::vector<uint16_t> indices = 
	{
		0, 1, 2, 2, 3, 0
	};

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

		commandBuffer = CommandBuffer::Create(Renderer::GetMaxFrames());
		
		framebuffer = Framebuffer::Create({ 1080, 720, ImageFormat::RGBA_8, false });
		renderPass = RenderPass::Create({ framebuffer });

		{
			PipelineProperties pipelineProperties;
			pipelineProperties.renderPass = renderPass;
			pipelineProperties.shader = Shader::Create("shaders/vert.spv", "shaders/frag.spv");
			pipelineProperties.layout =
			{
				{ShaderDataType::FLOAT3, "position"},
				{ShaderDataType::FLOAT3, "color"},
				{ShaderDataType::FLOAT2, "texCoord"},
			};

			pipeline = Pipeline::Create(pipelineProperties);
		}

		uboCamera = UniformBuffer::Create(sizeof(glm::mat4), Renderer::GetMaxFrames());
	}

	void EditorPanel::OnDestroy()
	{
		uboCamera->Destroy();
		
		scene->Destroy();

		pipeline->Destroy();
		framebuffer->Destroy();
		renderPass->Destroy();
	}

	void EditorPanel::Update()
	{
		if (needToUpdateFramebuffer)
		{
			framebuffer->Resize(pipeline, (uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
			camera.Resize(viewportSize.x, viewportSize.y);
			needToUpdateFramebuffer = false;
		}

		camera.Update();

		commandBuffer->BeginCommand();
				
		const auto& view = scene->GetWorld().view<TransformComponent, MeshComponent>();
		for (auto entity : view)
		{
			auto [transform, mesh] = view.get<TransformComponent, MeshComponent>(entity);

			const glm::mat4& rotationMat = glm::toMat4(glm::quat(glm::radians(transform.rotation)));

			transform.CalculateTransform();
			constants[0] = transform.transform;
			constants[1] = camera.GetProjectionMatrix() * camera.GetViewMatrix();
			pipeline->Bind(commandBuffer, (void*)constants);
			Renderer::SubmitGeometry(commandBuffer, mesh.vertexBuffer, mesh.indexBuffer);
		}

		pipeline->Unbind(commandBuffer);
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