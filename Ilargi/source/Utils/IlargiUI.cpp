#include "ilargipch.h"

#include "IlargiUI.h"

#include <imgui.h>

namespace Ilargi::UI
{
	void Checkbox(std::string_view name, bool* value)
	{
		float width = ImGui::GetWindowWidth();

		ImGui::Text(name.data());
		ImGui::SameLine(width - 25);

		ImGui::Checkbox("##checkbox", value);
	}

	void Slider(std::string_view name, float* value, float min, float max)
	{
		float width = ImGui::GetWindowWidth();

		ImGui::Text(name.data());
		ImGui::SameLine(width - 25);
		ImGui::SliderFloat("##slider", value, min, max);
	}
}