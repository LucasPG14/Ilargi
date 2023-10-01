#include "ilargipch.h"

#include "EditorPanel.h"

#include <imgui/imgui.h>
#include <glm/glm.hpp>

namespace Ilargi
{
	struct Vertex
	{
		glm::vec2 position;
		glm::vec3 color;
	};

	const std::vector<Vertex> vertices =
	{
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};
	const std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };

	EditorPanel::EditorPanel() : Panel("Editor Panel")
	{
	}

	EditorPanel::~EditorPanel()
	{
	}

	void EditorPanel::OnInit()
	{
		commandBuffer = CommandBuffer::Create(Renderer::GetMaxFrames());
		
		framebuffer = Framebuffer::Create({ 1080, 720, ImageFormat::RGBA_8, false });
		renderPass = RenderPass::Create({ framebuffer });

		{
			PipelineProperties pipelineProperties;
			pipelineProperties.renderPass = renderPass;
			pipelineProperties.shader = Shader::Create("shaders/vert.spv", "shaders/frag.spv");
			pipelineProperties.layout =
			{
				{ShaderDataType::FLOAT2, "position"},
				{ShaderDataType::FLOAT3, "color"}
			};

			pipeline = Pipeline::Create(pipelineProperties);
		}
		vertexBuffer = VertexBuffer::Create((void*)vertices.data(), static_cast<uint32_t>(vertices.size() * sizeof(Vertex)));
		indexBuffer = IndexBuffer::Create((void*)indices.data(), static_cast<uint32_t>(indices.size()));
	}

	void EditorPanel::OnDestroy()
	{
		vertexBuffer->Destroy();
		indexBuffer->Destroy();
		
		pipeline->Destroy();
		framebuffer->Destroy();
		renderPass->Destroy();
	}

	void EditorPanel::Update()
	{
		commandBuffer->BeginCommand();
		pipeline->Bind(commandBuffer, vertexBuffer, indexBuffer);
		
		Renderer::SubmitGeometry(commandBuffer, vertexBuffer, indexBuffer);

		pipeline->Unbind(commandBuffer);
		commandBuffer->EndCommand();
		
		Renderer::AddCommand(commandBuffer);
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

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::Begin("Viewport");
		ImGui::Image(framebuffer->TransitionImage(), ImGui::GetContentRegionAvail());
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::Begin("Scene Hierarchy", (bool*)0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar);
		ImGui::BeginChild("##", ImGui::GetContentRegionAvail());

		ImGui::EndChild();
		ImGui::End();
		ImGui::PopStyleVar();
		//auto padding = ImGui::GetStyle().WindowPadding;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::Begin("Inspector", (bool*)0);
		ImGui::BeginChild("##", ImGui::GetContentRegionAvail());

		ImGui::EndChild();
		ImGui::End();
		ImGui::PopStyleVar();

		ImVec2 size = ImGui::GetIO().DisplaySize;
		ImVec2 position = ImGui::GetWindowPos();
		//ImGui::SetNextWindowSize({ size.x , 30 });
		//ImGui::SetNextWindowPos({ position.x , position.y + size.y - 30 });
		//ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.06f, 0.06f, 0.06f, 0.94f));
		ImGui::Begin("Bottom bar", (bool*)0, ImGuiWindowFlags_NoTitleBar);
		ImGui::End();
		//ImGui::PopStyleColor();

		ImGui::End();
	}
	
	void EditorPanel::OnEvent(Event& event)
	{
	}
}