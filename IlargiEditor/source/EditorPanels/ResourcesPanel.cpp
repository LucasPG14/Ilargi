#include "ilargipch.h"
#include "ResourcesPanel.h"

#include "Base/UUID.h"
#include "Resources/ResourceManager.h"

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

			ImGui::Button(filename.c_str(), { cell, cell });
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("RESOURCE", &assets[path], sizeof(assets[path]));
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			{
				if (file.is_directory()) actualDir /= relative;
			}
			ImGui::Text(filename.c_str());
			ImGui::NextColumn();
		}

		ImGui::Columns(1);

		if (ImGui::BeginPopupContextWindow("##Hierarchypopup"))
		{
			if (ImGui::MenuItem("Create Material"))
			{

			}
			ImGui::EndPopup();
		}

		ImGui::End();
	}
	
	void ResourcesPanel::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowDropEvent>(ILG_BIND_FN(ResourcesPanel::OnDropEvent));
	}
	
	bool ResourcesPanel::OnDropEvent(WindowDropEvent& event)
	{
		const std::vector<std::filesystem::path>& paths = event.GetPaths();

		for (int i = 0; i < paths.size(); ++i)
		{
			ResourceManager::ImportResource(paths[i]);
		}

		assets.clear();
		const auto& assetsMap = ResourceManager::GetResourcesMetadata();

		for (auto& [uuid, metadata] : assetsMap)
		{
			assets[metadata.filepath] = uuid;
		}

		return true;
	}
}