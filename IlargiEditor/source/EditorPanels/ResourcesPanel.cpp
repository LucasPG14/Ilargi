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
		
		for (const auto& iterator : std::filesystem::directory_iterator(actualDir))
		{
			if (iterator.is_directory())
			{
				ImGui::ColorButton("Folder", { 1.0f, 0.0, 0.0f, 0.2f });
			}
			else
			{
				ImGui::ColorButton("File", { 0.0f, 0.0, 1.0f, 0.2f });
			}
		}

		ImGui::End();
	}
}