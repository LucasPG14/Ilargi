#include "ilargipch.h"
#include "ResourcesPanel.h"

#include "Base/UUID.h"
#include "Resources/ResourceManager.h"
#include "Resources/Texture.h"

#include "Utils/FileSystem.h"

#include <imgui/imgui.h>

namespace Ilargi
{
	namespace Utils
	{
		std::string GetStringFromResourceType(ResourceType type)
		{
			switch (type)
			{
			case Ilargi::ResourceType::NONE: return "UNKNOWN";
			case Ilargi::ResourceType::MODEL: return "MODEL";
			case Ilargi::ResourceType::TEXTURE2D: return "TEXTURE2D";
			case Ilargi::ResourceType::MATERIAL: return "MATERIAL";
			case Ilargi::ResourceType::SCENE: return "SCENE";
			}

			return "Unknown";
		}
	}

	ResourcesPanel::ResourcesPanel()
	{
		actualDir = "Resources";

		folderIcon = Texture2D::Create("Engine/Textures/Folder.png");
		fileIcon = Texture2D::Create("Engine/Textures/File2.png");

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
			auto s = actualDir.parent_path();
			ImGui::SameLine();
			ImGui::Text(dir.string().c_str());
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			{
				bool ret = true;
				ret = false;
			}

			ImGui::SameLine();
			ImGui::Text("/");
		}
		
		if (!search.empty())
			RecursiveDirectory();
		else 
			NormalDirectory();

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
			ResourceManager::ImportResource(actualDir, paths[i]);
		}

		ResourceManager::SaveResourceRegistry();

		assets.clear();
		const auto& assetsMap = ResourceManager::GetResourcesMetadata();

		for (auto& [uuid, metadata] : assetsMap)
		{
			assets[metadata.filepath] = uuid;
		}

		return true;
	}
	
	void ResourcesPanel::NormalDirectory()
	{
		constexpr float cellX = 128.0f;
		constexpr float cellY = 190.0f;

		int columns = int(ImGui::GetContentRegionAvail().x / cellX);

		ImGui::Columns(columns, (const char*)0, false);

		for (const auto& file : std::filesystem::directory_iterator(actualDir))
		{
			const auto& path = file.path();
			const auto& relative = std::filesystem::relative(path, actualDir);
			const auto& filename = path.stem().string();

			if (file.is_directory())
			{
				ImGui::Image((ImTextureID)folderIcon->GetID(), { cellX, cellX });

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
					actualDir /= relative;

				ImVec2 textSize = ImGui::CalcTextSize(filename.c_str());
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cellX - textSize.x) * 0.5f);
				ImGui::Text(filename.c_str());
			}
			else
			{
				//if (path.extension().string() != "itex" || path.extension().string() != "imodel")
				//	continue;

				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

				ImVec4 colorBg = { 0.43f, 0.43f, 0.50f, 0.50f };
				if (selectedFile == path)
					colorBg = { 0.26f, 0.59f, 0.98f, 0.40f };

				ImGui::PushStyleColor(ImGuiCol_ChildBg, colorBg);
				ImGui::PushStyleColor(ImGuiCol_Border, colorBg);
				if (ImGui::BeginChild(path.string().c_str(), {cellX, cellY}, true, ImGuiWindowFlags_NoDecoration))
				{
					if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
						selectedFile = path;

					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8.0f, 8.0f });
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
					{	
						ImGui::SetDragDropPayload("RESOURCE", &assets[path], sizeof(assets[path]));
						ImGui::Text(path.filename().string().c_str());
						ImGui::EndDragDropSource();
					}
					ImGui::PopStyleVar();
					
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
					//ImGui::Button(filename.c_str(), {cellX, cellX});
					ImGui::Image((ImTextureID)fileIcon->GetID(), { cellX, cellX });
					ImGui::PopStyleVar();

					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
					ImGui::Text(filename.c_str());
					if (assets.find(path) != assets.end())
					{
						UUID uuid = assets[path];
						const ResourceMetadata& metadata = ResourceManager::GetMetadata(uuid);
					
						std::string resType = Utils::GetStringFromResourceType(metadata.type);

						ImVec2 textSize = ImGui::CalcTextSize(resType.c_str());
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cellX - textSize.x - 5.0f));
						ImGui::SetCursorPosY(cellY - textSize.y - 5.0f);
						ImGui::Text(resType.c_str());
					}

					ImGui::EndChild();
				}
				ImGui::PopStyleColor(2);
				ImGui::PopStyleVar(2);
			}

			ImGui::NextColumn();
		}
	}
	
	void ResourcesPanel::RecursiveDirectory()
	{
		constexpr float cellX = 128.0f;
		constexpr float cellY = 190.0f;

		int columns = int(ImGui::GetContentRegionAvail().x / cellX);

		ImGui::Columns(columns, (const char*)0, false);

		for (const auto& file : std::filesystem::recursive_directory_iterator(actualDir))
		{
			const auto& path = file.path();
			const auto& relative = std::filesystem::relative(path, actualDir);
			const auto& filename = path.stem().string();

			if (file.is_directory())
				continue;


			//if (path.extension().string() != "itex" || path.extension().string() != "imodel")
			//	continue;

			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
			ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.43f, 0.43f, 0.50f, 0.50f });
			if (ImGui::BeginChild(path.string().c_str(), { cellX, cellY }, true, ImGuiWindowFlags_NoDecoration))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8.0f, 8.0f });
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					ImGui::SetDragDropPayload("RESOURCE", &assets[path], sizeof(assets[path]));
					ImGui::Text(path.filename().string().c_str());
					ImGui::EndDragDropSource();
				}
				ImGui::PopStyleVar();

				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
				//ImGui::Button(filename.c_str(), { cellX, cellX });
				ImGui::Image((ImTextureID)fileIcon->GetID(), { cellX, cellX });
				ImGui::PopStyleVar();

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
				ImGui::Text(filename.c_str());
				if (assets.find(path) != assets.end())
				{
					UUID uuid = assets[path];
					const ResourceMetadata& metadata = ResourceManager::GetMetadata(uuid);

					std::string resType = Utils::GetStringFromResourceType(metadata.type);

					ImVec2 textSize = ImGui::CalcTextSize(resType.c_str());
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cellX - textSize.x - 5.0f));
					ImGui::SetCursorPosY(cellY - textSize.y - 5.0f);
					ImGui::Text(resType.c_str());
				}

				ImGui::EndChild();
			}
			ImGui::PopStyleColor();
			ImGui::PopStyleVar(2);

			ImGui::NextColumn();
		}
	}
}