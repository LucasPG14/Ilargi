#include "ilargipch.h"
#include "ResourcesPanel.h"

#include <imgui/imgui.h>

namespace Ilargi
{
	ResourcesPanel::ResourcesPanel()
	{
		actualDir = "assets";
	}

	ResourcesPanel::~ResourcesPanel()
	{
	}

	void ResourcesPanel::Render()
	{
		ImGui::Begin("Resources Panel");

		constexpr float cell = 96.0f;

		int columns = int(ImGui::GetContentRegionMax().x / cell);

		ImGui::Columns(columns, (const char*)0, false);

		for (const auto& file : std::filesystem::directory_iterator(actualDir))
		{
			const auto& path = file.path();
			const auto& relative = std::filesystem::relative(path, "assets");
			const auto& filename = path.stem().string();
			//const auto& file = path.;

			if (file.is_directory())
			{
				ImGui::Button(filename.c_str(), { cell, cell });
			}
			else
			{
				ImGui::Button(filename.c_str(), { cell, cell });
			}
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			{
				if (file.is_directory()) actualDir /= relative;
			}
			ImGui::Text(filename.c_str());
			ImGui::NextColumn();
		}

		ImGui::Columns(1);

		//bool focused = ImGui::IsWindowFocused();
		if (!ImGui::IsAnyItemHovered() /*&& focused*/ && ImGui::BeginPopupContextWindow("##Hierarchypopup"))
		{
			if (ImGui::MenuItem("Create Material"))
			{

			}
			ImGui::EndPopup();
		}

		ImGui::End();
	}
}