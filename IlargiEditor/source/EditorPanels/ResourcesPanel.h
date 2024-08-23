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
		void OnEvent(Event& event);

		void RefreshAssets();

	private:
		bool OnDropEvent(WindowDropEvent& event);
		bool OnKeyPressedEvent(KeyPressedEvent& event);

		void NormalDirectory();
		void RecursiveDirectory();
	private:
		std::filesystem::path actualDir;
		
		std::filesystem::path selectedFile;

		std::string search;

		std::unordered_map<std::filesystem::path, UUID> assets;

		std::shared_ptr<Texture2D> folderIcon;
		std::shared_ptr<Texture2D> fileIcon;
	};
}