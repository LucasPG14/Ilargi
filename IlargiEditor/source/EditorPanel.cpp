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
		pipeline = Pipeline::Create({ Shader::Create("shaders/vert.spv", "shaders/frag.spv"), renderPass});

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


		ImGui::Begin("Scene Hierarchy");
		//ImGui::Image(framebuffer->GetID(), {100, 100});
		bool hola = true;
		UI::Checkbox("My checkbox", &hola);

		float val = 5.0f;
		UI::Slider("My slider", &val);
		ImGui::End();

		ImGui::Begin("Inspector");
		ImGui::End();

		//if (ImGui::BeginMainMenuBar())
		//{
		//	if (ImGui::BeginMenu("File"))
		//	{
		//		if (ImGui::MenuItem("New Project"))
		//		{
		//
		//		}
		//		ImGui::EndMenu();
		//	}
		//	ImGui::EndMainMenuBar();
		//}

		ImGui::End();
	}
	
	void EditorPanel::OnEvent(Event& event)
	{
	}
}