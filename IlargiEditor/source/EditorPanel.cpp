#include "EditorPanel.h"

#include <imgui/imgui.h>

namespace Ilargi
{
	EditorPanel::EditorPanel() : Panel("Editor Panel")
	{
	}

	EditorPanel::~EditorPanel()
	{
	}

	void EditorPanel::OnInit()
	{
	}

	void EditorPanel::Update()
	{
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

	void EditorPanel::OnDestroy()
	{
	}
	
	void EditorPanel::OnEvent(Event& event)
	{
	}
}