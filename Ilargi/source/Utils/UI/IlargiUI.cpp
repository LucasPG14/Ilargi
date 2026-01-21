#include "ilargipch.h"

#include "IlargiUI.h"

#include <imgui.h>

namespace Ilargi::UI
{
	void Checkbox(std::string_view name, bool* value)
	{
        ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextPadding, { 80.0f, 80.0f });
        ImGui::SetCursorPos({ 8.0f, ImGui::GetCursorPosY() + 8.0f});
        ImGui::Text(name.data());
        ImGui::PopStyleVar();

        float width{ ImGui::GetWindowWidth() };
		ImGui::SameLine();
        ImGui::SetCursorPos({ width - 25, ImGui::GetCursorPosY() - 3.0f });
		ImGui::Checkbox("##checkbox", value);
	}

	void Slider(std::string_view name, float* value, float min, float max)
	{
        float width{ ImGui::GetWindowWidth() };

		ImGui::Text(name.data());
		ImGui::SameLine(width - 25);
		ImGui::SliderFloat("##slider", value, min, max);
	}

    bool BeginTreeNode(void* id, std::string name, ImGuiTreeNodeFlags flags)
    {
        ImGui::PushID(id);
        return ImGui::TreeNodeEx(name.c_str(), flags);
    }

    void EndTreeNode(void* id, bool open)
    {
        if (open)
            ImGui::TreePop();

        ImGui::PopID();
    }

    bool BeginCollapsingHeader(void* id, std::string name)
    {
        ImGui::PushID(id);

        return ImGui::CollapsingHeader(name.c_str());
    }

    void EndCollapsingHeader(void* id)
    {
        ImGui::TreePop();
    }
    
    void IlargiStyle()
    {
        ImGuiStyle& style{ ImGui::GetStyle() };
        ImVec4* colors{ style.Colors };

        colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.12f, 0.94f);

        colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.17f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.65f, 0.10f, 0.12f, 0.35f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.65f, 0.10f, 0.12f, 0.60f);

        colors[ImGuiCol_Button] = ImVec4(0.65f, 0.10f, 0.12f, 0.30f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.85f, 0.15f, 0.18f, 0.55f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.55f, 0.07f, 0.09f, 1.00f);

        colors[ImGuiCol_Header] = ImVec4(0.65f, 0.10f, 0.12f, 0.25f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.85f, 0.15f, 0.18f, 0.45f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.65f, 0.10f, 0.12f, 0.75f);

        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_Border] = { 1.0f, 0.0f, 0.0f, 0.03f };
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        colors[ImGuiCol_Tab] = ImVec4(0.13f, 0.13f, 0.14f, 1.00f);
        colors[ImGuiCol_TabHovered] = { 0.85f, 0.15f, 0.18f, 0.40f };
        colors[ImGuiCol_TabActive] = ImVec4(0.64f, 0.10f, 0.12f, 0.85f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.13f, 0.13f, 0.14f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.35f, 0.08f, 0.09f, 1.00f);
       
        colors[ImGuiCol_TitleBg] = ImVec4(0.11f, 0.11f, 0.12f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.11f, 0.85f);

        colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.15f, 0.18f, 1.00f);

        colors[ImGuiCol_SliderGrab] = ImVec4(0.75f, 0.10f, 0.12f, 0.80f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.90f, 0.15f, 0.18f, 1.00f);

        colors[ImGuiCol_ResizeGrip] = ImVec4(0.75f, 0.10f, 0.12f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.90f, 0.15f, 0.18f, 0.60f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.90f, 0.15f, 0.18f, 1.00f);

        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.90f, 0.15f, 0.18f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 0.20f, 0.22f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.90f, 0.15f, 0.18f, 0.80f);

        colors[ImGuiCol_PlotLines] = ImVec4(0.90f, 0.15f, 0.18f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.25f, 0.28f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.15f, 0.18f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.90f, 0.25f, 0.28f, 1.00f);

        colors[ImGuiCol_DockingPreview] = ImVec4(0.90f, 0.15f, 0.18f, 0.60f);

        style.FrameRounding = 3;
        style.FramePadding = ImVec2(6,4);
        style.IndentSpacing = 18;
        style.TabRounding = 3;
        style.WindowRounding = 4;
        style.TabBorderSize = 1;
        style.FrameBorderSize = 1;
    }
}