#include "ilargipch.h"

#include "LauncherPanel.h"
#include "Utils/FileSystem.h"

#include <ImGUI/imgui.h>

namespace Ilargi
{
	LauncherPanel::LauncherPanel()
	{
	}

	LauncherPanel::~LauncherPanel()
	{
	}
	
	void LauncherPanel::OnInit()
	{
	}
	
	void LauncherPanel::OnDestroy()
	{
	}
	
	void LauncherPanel::Update(float aDeltaTime)
	{
	}
	
	void LauncherPanel::RenderImGui()
	{
		static bool dockspaceOpen{ true };
		static ImGuiDockNodeFlags dockspaceFlags{ ImGuiDockNodeFlags_None };

		ImGuiWindowFlags windowFlags{ ImGuiWindowFlags_NoDocking };
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

		if (ImGui::Button("fgd"))
		{
			FileSystem::OpenApp();
		}

		static char buffer[128] {""};
		if (ImGui::InputText("##Directory", buffer, sizeof(buffer)))
		{
			mDirectory = buffer;
		}

		ImGui::End();
	}
	
	void LauncherPanel::OnEvent(Event& aEvent)
	{
	}
}