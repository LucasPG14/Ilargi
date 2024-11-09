#pragma once

#include "Events/Event.h"
#include "Events/WindowEvents.h"
#include "Events/KeyEvents.h"

namespace Ilargi
{
	class UUID;
	class Texture2D;

	class ResourcesPanel
	{
	public:
		ResourcesPanel();
		~ResourcesPanel();

		void Render();
		void OnEvent(Event& aEvent);

		void RefreshAssets();

	private:
		bool OnDropEvent(WindowDropEvent& aEvent);
		bool OnKeyPressedEvent(KeyPressedEvent& aEvent);

		void NormalDirectory();
		void RecursiveDirectory();
	private:
		std::filesystem::path mActualDir;
		
		std::filesystem::path mSelectedFile;

		std::string mSearch;

		std::unordered_map<std::filesystem::path, UUID> mResources;

		std::shared_ptr<Texture2D> mFolderIcon;
		std::shared_ptr<Texture2D> mFileIcon;

		bool mResourcesPanelFocused;
	};
}