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

		float width = ImGui::GetWindowWidth();
		ImGui::SameLine();
        ImGui::SetCursorPos({ width - 25, ImGui::GetCursorPosY() - 3.0f });
		ImGui::Checkbox("##checkbox", value);
	}

	void Slider(std::string_view name, float* value, float min, float max)
	{
		float width = ImGui::GetWindowWidth();

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
        ImGuiStyle* style = &ImGui::GetStyle();
        ImVec4* colors = style->Colors;

        ImVec4 bgColor = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        ImVec4 borderColor = ImVec4(0.05f, 0.05f, 0.05f, 1.0f);
        ImVec4 titleBg = ImVec4(51.0f / 255.0f, 51.0f / 255.0f, 51.0f / 255.0f, 0.20f);
        ImVec4 titleBgActive = ImVec4(77.0f / 255.0f, 77.0f / 255.0f, 77.0f / 255.0f, 0.30f);

        style->DockingSeparatorSize = 3.0f;

        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg] = bgColor;
        colors[ImGuiCol_ChildBg] = titleBgActive;
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_Border] = { 1.0f, 0.0f, 0.0f, 0.03f };
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = { 1.0f, 0.0f, 0.0f, 0.10f };
        colors[ImGuiCol_FrameBgHovered] = { 1.0f, 0.0f, 0.0f, 0.30f };
        colors[ImGuiCol_FrameBgActive] = { 1.0f, 0.0f, 0.0f, 0.55f };
        colors[ImGuiCol_TitleBg] = { 1.0f, 0.0f, 0.0f, 0.03f };
        colors[ImGuiCol_TitleBgActive] = { 1.0f, 0.0f, 0.0f, 0.03f };
        colors[ImGuiCol_TitleBgCollapsed] = borderColor;
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        colors[ImGuiCol_Header] = titleBg;
        colors[ImGuiCol_HeaderHovered] = titleBg;
        colors[ImGuiCol_HeaderActive] = titleBg;
        colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = { 1.0f, 0.0f, 0.0f, 1.0f };
        colors[ImGuiCol_ResizeGrip] = {1.0f, 0.0f, 0.0f, 1.0f};
        colors[ImGuiCol_ResizeGripHovered] = { 1.0f, 0.0f, 0.0f, 0.2f };
        colors[ImGuiCol_ResizeGripActive] = { 1.0f, 0.0f, 0.0f, 0.4f };
        colors[ImGuiCol_Tab] = { 1.0f, 0.0f, 0.0f, 0.10f };
        colors[ImGuiCol_TabHovered] = { 1.0f, 0.0f, 0.0f, 0.10f };
        colors[ImGuiCol_TabActive] = { 1.0f, 0.0f, 0.0f, 0.10f };
        colors[ImGuiCol_TabUnfocused] = titleBgActive;
        colors[ImGuiCol_TabUnfocusedActive] = { 1.0f, 0.0f, 0.0f, 0.10f };
        //colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}
    
    void IlargiStyle2()
    {
        ImGuiStyle* style = &ImGui::GetStyle();
        ImVec4* colors = style->Colors;

        ImVec4 bgColor = ImVec4(16.0f / 255.0f, 20.0f / 255.0f, 45.0f / 255.0f, 1.0f);
        ImVec4 borderColor = ImVec4(146.0f / 255.0f, 171.0f / 255.0f, 1.0f, 1.0f);
        ImVec4 titleBg = ImVec4(51.0f / 255.0f, 51.0f / 255.0f, 51.0f / 255.0f, 0.20f);
        ImVec4 titleBgActive = ImVec4(77.0f / 255.0f, 77.0f / 255.0f, 77.0f / 255.0f, 0.30f);

        style->DockingSeparatorSize = 3.0f;

        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg] = bgColor;
        colors[ImGuiCol_ChildBg] = titleBgActive;
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_Border] = { 1.0f, 0.0f, 0.0f, 0.03f };
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = { 1.0f, 0.0f, 0.0f, 0.10f };
        colors[ImGuiCol_FrameBgHovered] = { 1.0f, 0.0f, 0.0f, 0.30f };
        colors[ImGuiCol_FrameBgActive] = { 1.0f, 0.0f, 0.0f, 0.55f };
        colors[ImGuiCol_TitleBg] = { 6.0f / 255.0f, 10.0f / 255.0f, 35.0f / 255.0f, 1.0f };
        colors[ImGuiCol_TitleBgActive] = { 6.0f / 255.0f, 10.0f / 255.0f, 35.0f / 255.0f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed] = borderColor;
        colors[ImGuiCol_MenuBarBg] = ImVec4(16.0f / 255.0f, 20.0f / 255.0f, 45.0f / 255.0f, 1.0f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_Button] = borderColor;
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        colors[ImGuiCol_Header] = borderColor;
        colors[ImGuiCol_HeaderHovered] = titleBg;
        colors[ImGuiCol_HeaderActive] = titleBg;
        colors[ImGuiCol_Separator] = borderColor;
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = { 1.0f, 0.0f, 0.0f, 1.0f };
        colors[ImGuiCol_ResizeGrip] = { 1.0f, 0.0f, 0.0f, 1.0f };
        colors[ImGuiCol_ResizeGripHovered] = { 1.0f, 0.0f, 0.0f, 0.2f };
        colors[ImGuiCol_ResizeGripActive] = { 1.0f, 0.0f, 0.0f, 0.4f };
        colors[ImGuiCol_Tab] = { 16.0f / 255.0f, 20.0f / 255.0f, 45.0f / 255.0f, 1.0f };
        colors[ImGuiCol_TabHovered] = { 16.0f / 255.0f, 20.0f / 255.0f, 45.0f / 255.0f, 1.0f };
        colors[ImGuiCol_TabActive] = { 16.0f / 255.0f, 20.0f / 255.0f, 45.0f / 255.0f, 1.0f };
        colors[ImGuiCol_TabUnfocused] = { 16.0f / 255.0f, 20.0f / 255.0f, 45.0f / 255.0f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive] = { 16.0f / 255.0f, 20.0f / 255.0f, 45.0f / 255.0f, 1.0f };
        //colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }
}