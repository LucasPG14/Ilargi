#include "ilargipch.h"
#include "ResourcesPanel.h"

#include "Base/UUID.h"
#include "Resources/ResourceManager.h"
#include "Resources/Texture.h"

#include "Utils/FileSystem.h"

#include <imgui/imgui.h>

namespace Ilargi
{
	ResourcesPanel::ResourcesPanel()
	{
		actualDir = "Assets";

		folderIcon = Texture2D::Create("EngineResources/Icon.png");

		ResourceManager::LoadResourceRegistry();

		// TODO: Change this
		const auto& assetsMap = ResourceManager::GetResourcesMetadata();

		for (auto& [uuid, metadata] : assetsMap)
		{
			assets[metadata.filepath] = uuid;
		}
	}

	ResourcesPanel::~ResourcesPanel()
	{
		folderIcon->Destroy();
	}

	void ResourcesPanel::Render()
	{
		ImGui::Begin("Resources Panel");

		if (ImGui::ArrowButton("Arrow", ImGuiDir_Left))
		{
			actualDir = actualDir.has_parent_path() ? actualDir.parent_path() : actualDir;
		}

		ImGui::SameLine();

		if (ImGui::ArrowButton("Arrow", ImGuiDir_Right))
		{
		}
		ImGui::SameLine();

		ImGui::SetNextItemWidth(200.0f);
		char* buf = search.data();
		ImGui::InputText("##Search...", buf, sizeof(buf));
		search = buf;

		for (auto dir : actualDir)
		{
			ImGui::SameLine();
			if (ImGui::Button(dir.string().c_str(), { 0, 0 }))
			{
			}

			ImGui::SameLine();
			ImGui::Text("/");
		}

		constexpr float cell = 128.0f;

		int columns = int(ImGui::GetContentRegionMax().x / cell);

		ImGui::Columns(columns, (const char*)0, false);

		for (const auto& file : std::filesystem::directory_iterator(actualDir))
		{
			const auto& path = file.path();
			const auto& relative = std::filesystem::relative(path, actualDir);
			const auto& filename = path.stem().string();

			if (file.is_directory())
				ImGui::ImageButton((ImTextureID)folderIcon->GetID(), { cell, cell });
			else
				ImGui::Button(filename.c_str(), { cell, cell });
			
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("RESOURCE", &assets[path], sizeof(assets[path]));
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			{
				if (file.is_directory()) 
					actualDir /= relative;
			}
			ImGui::Text(filename.c_str());
			ImGui::NextColumn();
		}

		ImGui::Columns(1);

		if (ImGui::BeginPopupContextWindow("##HierarchyPopup"))
		{
			if (ImGui::MenuItem("Create Material"))
			{
				ResourceMetadata metadata;
				metadata.type = ResourceType::MATERIAL;
				metadata.filepath = actualDir / "NewMaterial.ires";
				metadata.sourceFile = "";

				ResourceManager::RegisterResource(metadata);

				Buffer buffer;
				buffer.size = 0;
				buffer.data = nullptr;
				FileSystem::WriteBinaryFile(metadata.filepath, buffer);
			}
			if (ImGui::MenuItem("Create Folder"))
			{
				std::filesystem::create_directory(actualDir / "New Folder");
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