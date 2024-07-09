#pragma once

#include "Events/Event.h"
#include "Events/WindowEvents.h"

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

	private:
		bool OnDropEvent(WindowDropEvent& event);

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