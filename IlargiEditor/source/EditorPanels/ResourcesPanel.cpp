#include "ilargipch.h"
#include "ResourcesPanel.h"

#include "MaterialPanel.h"

#include "Base/UUID.h"
#include "Base/Input.h"
#include "Resources/ResourceManager.h"
#include "Resources/Texture.h"
#include "Resources/Material.h"

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
			case Ilargi::ResourceType::NONE:		return "UNKNOWN";
			case Ilargi::ResourceType::MODEL:		return "MODEL";
			case Ilargi::ResourceType::TEXTURE2D:	return "TEXTURE2D";
			case Ilargi::ResourceType::MATERIAL:	return "MATERIAL";
			case Ilargi::ResourceType::SCENE:		return "SCENE";
			}

			return "Unknown";
		}

		bool IsResourceValid(std::string extension)
		{
			if (extension == std::string(".imodel"))
				return true;
			if (extension == std::string(".itex"))
				return true;
			if (extension == std::string(".ilargi"))
				return true;
			if (extension == std::string(".imat"))
				return true;

			return false;
		}
	}

	ResourcesPanel::ResourcesPanel()
	{
		mActualDir = "Resources";

		mFolderIcon = Texture2D::Create("Engine/Textures/Folder.png");
		mFileIcon = Texture2D::Create("Engine/Textures/File2.png");

		mMaterialPanel = new MaterialPanel();

		ResourceManager::LoadResourceRegistry();

		RefreshAssets();
	}

	ResourcesPanel::~ResourcesPanel()
	{
	}

	void ResourcesPanel::Render()
	{
		if (ImGui::Begin("Resources Panel"))
		{
			//mResourcesPanelFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);

			if (ImGui::ArrowButton("Arrow", ImGuiDir_Left))
			{
				mActualDir = mActualDir.has_parent_path() ? mActualDir.parent_path() : mActualDir;
			}

			ImGui::SameLine();

			if (ImGui::ArrowButton("Arrow", ImGuiDir_Right))
			{
			}
			ImGui::SameLine();

			ImGui::SetNextItemWidth(200.0f);
			char* buf{ mSearch.data() };
			ImGui::InputTextWithHint("##Search...", "Search...", buf, sizeof(buf));
			mSearch = buf;

			for (auto dir : mActualDir)
			{
				auto actualDirParent{ mActualDir.parent_path() };
				ImGui::SameLine();
				ImGui::Text(dir.string().c_str());
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					uint64_t end{ mActualDir.string().find(dir.string()) + dir.string().length() };
					mActualDir = mActualDir.string().substr(0, end);
					break;
				}

				ImGui::SameLine();
				ImGui::Text("/");
			}

			if (!mSearch.empty())
				RecursiveDirectory();
			else
				NormalDirectory();

			ImGui::Columns(1);

			if (!mSelectedFile.empty() && mResourcesPanelFocused && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				mSelectedFile.clear();

			//if (ImGui::BeginPopupContextWindow("##HierarchyPopup"))
			//{
			//	if (ImGui::MenuItem("Create Folder"))
			//	{
			//		std::filesystem::create_directory(mActualDir / "New Folder");
			//	}
			//	ImGui::EndPopup();
			//}

			ImGui::End();
		}

		mMaterialPanel->Render();
	}
	
	void ResourcesPanel::OnEvent(Event& aEvent)
	{
		EventDispatcher dispatcher(aEvent);

		dispatcher.Dispatch<WindowDropEvent>(ILG_BIND_FN(ResourcesPanel::OnDropEvent));
		dispatcher.Dispatch<KeyPressedEvent>(ILG_BIND_FN(ResourcesPanel::OnKeyPressedEvent));
	}

	void ResourcesPanel::RefreshAssets()
	{
		mResources.clear();
		const auto& assetsMap{ ResourceManager::GetResourcesMetadata() };

		for (auto& [uuid, metadata] : assetsMap)
		{
			mResources[metadata.filepath] = uuid;
		}
	}
	
	bool ResourcesPanel::OnDropEvent(WindowDropEvent& aEvent)
	{
		const std::vector<std::filesystem::path>& paths{ aEvent.GetPaths() };

		for (uint32_t i { 0U }; i < paths.size(); ++i)
		{
			ResourceManager::ImportResource(mActualDir, paths[i]);
		}

		RefreshAssets();

		return true;
	}

	bool ResourcesPanel::OnKeyPressedEvent(KeyPressedEvent& aEvent)
	{
		switch (aEvent.GetKey())
		{
		case KeyCode::DELETE:
		{
			if (mResourcesPanelFocused && !mSelectedFile.empty())
			{
				std::filesystem::remove(mSelectedFile);
				UUID resourceUUID{ mResources[mSelectedFile] };
				ResourceManager::RemoveResource(resourceUUID);
			}
			break;
		}
		}

		return true;
	}
	
	void ResourcesPanel::NormalDirectory()
	{
		constexpr float cellX { 132.0f };
		constexpr float cellY { 190.0f };

		int columns{ int(ImGui::GetContentRegionAvail().x / cellX) };

		ImGui::Columns(columns, (const char*)0, false);

		for (const auto& file : std::filesystem::directory_iterator(mActualDir))
		{
			const auto& path{ file.path() };
			const auto& relative{ std::filesystem::relative(path, mActualDir) };
			const auto& filename{ path.stem().string() };

			if (file.is_directory())
			{
				ImGui::Image((ImTextureID)mFolderIcon->GetID(), { cellX, cellX });

				if (ImGui::IsItemHovered())
				{
					if (ImGui::IsMouseDoubleClicked(0))
					{
						mActualDir /= relative;
					}
					else if (ImGui::IsMouseClicked(0))
					{
						mSelectedFile = path;
					}
				}	

				ImVec2 textSize{ ImGui::CalcTextSize(filename.c_str()) };
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cellX - textSize.x) * 0.5f);
				ImGui::Text(filename.c_str());
			}
			else
			{
				if (!Utils::IsResourceValid(path.extension().string()))
					continue;

				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

				ImVec4 colorBg { 0.43f, 0.43f, 0.50f, 0.50f };
				if (mSelectedFile == path)
					colorBg = { 0.26f, 0.59f, 0.98f, 0.40f };

				ImGui::PushStyleColor(ImGuiCol_ChildBg, colorBg);
				ImGui::PushStyleColor(ImGuiCol_Border, colorBg);
				if (ImGui::BeginChild(path.string().c_str(), {cellX, cellY}, true, ImGuiWindowFlags_NoDecoration))
				{
					if (ImGui::IsWindowHovered())
					{
						if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							if (ResourceManager::GetMetadata(mResources[path]).type == ResourceType::MATERIAL)
							{
								mMaterialPanel->SetMaterial(std::static_pointer_cast<Material>(ResourceManager::GetResource(mResources[path])));
							}
						}
						else if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
						{
							mSelectedFile = path;
						}
						else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
						{
							mSelectedFile = path;
							// TODO: Pop up with options for this file
						}
					}

					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8.0f, 8.0f });
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
					{	
						const auto& resourceMetadata{ ResourceManager::GetMetadata(mResources[path]) };
						ImGui::SetDragDropPayload(Utils::GetStringFromResourceType(resourceMetadata.type).c_str(), &mResources[path], sizeof(UUID));
						ImGui::Text(path.filename().string().c_str());
						ImGui::EndDragDropSource();
					}
					ImGui::PopStyleVar();
					
					//ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
					//ImGui::Button(filename.c_str(), {cellX, cellX});
					if (ResourceManager::GetMetadata(mResources[path]).type == ResourceType::TEXTURE2D)
					{
						ImGui::Image((ImTextureID)std::static_pointer_cast<Texture2D>(ResourceManager::GetResource(mResources[path]))->GetID(), { 132, cellX });
					}
					else
					{
						ImGui::Image((ImTextureID)mFileIcon->GetID(), { cellX, cellX });
					}
					//ImGui::PopStyleVar();

					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
					ImGui::Text(filename.c_str());
					if (mResources.find(path) != mResources.end())
					{
						UUID uuid{ mResources[path] };
						const ResourceMetadata& metadata{ ResourceManager::GetMetadata(uuid) };
					
						std::string resType{ Utils::GetStringFromResourceType(metadata.type) };

						ImVec2 textSize{ ImGui::CalcTextSize(resType.c_str()) };
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cellX - textSize.x - 5.0f));
						ImGui::SetCursorPosY(cellY - textSize.y - 5.0f);
						ImGui::Text(resType.c_str());
					}
				}
				ImGui::EndChild();
				ImGui::PopStyleColor(2);
				ImGui::PopStyleVar(2);
			}

			ImGui::NextColumn();
		}
	}
	
	void ResourcesPanel::RecursiveDirectory()
	{
		constexpr float cellX { 128.0f };
		constexpr float cellY { 190.0f };

		int columns{ int(ImGui::GetContentRegionAvail().x / cellX) };

		ImGui::Columns(columns, (const char*)0, false);
		std::regex pattern(mSearch, std::regex_constants::icase);

		for (const auto& file : std::filesystem::recursive_directory_iterator(mActualDir))
		{
			const auto& path{ file.path() };
			const auto& relative{ std::filesystem::relative(path, mActualDir) };
			const auto& filename{ path.stem().string() };
			const auto& extension{ path.extension().string() };

			if (file.is_directory() || !Utils::IsResourceValid(extension) || !std::regex_search(filename, pattern))
				continue;

			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
			ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.43f, 0.43f, 0.50f, 0.50f });
			if (ImGui::BeginChild(path.string().c_str(), { cellX, cellY }, true, ImGuiWindowFlags_NoDecoration))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8.0f, 8.0f });
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					const auto& resourceMetadata{ ResourceManager::GetMetadata(mResources[path]) };
					ImGui::SetDragDropPayload(Utils::GetStringFromResourceType(resourceMetadata.type).c_str(), &mResources[path], sizeof(UUID));
					ImGui::Text(filename.c_str());
					ImGui::EndDragDropSource();
				}
				ImGui::PopStyleVar();

				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
				//ImGui::Button(filename.c_str(), { cellX, cellX });
				ImGui::Image((ImTextureID)mFileIcon->GetID(), { cellX, cellX });
				ImGui::PopStyleVar();

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
				ImGui::Text(filename.c_str());
				if (mResources.find(path) != mResources.end())
				{
					UUID uuid{ mResources[path] };
					const ResourceMetadata& metadata{ ResourceManager::GetMetadata(uuid) };

					std::string resType{ Utils::GetStringFromResourceType(metadata.type) };

					ImVec2 textSize{ ImGui::CalcTextSize(resType.c_str()) };
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