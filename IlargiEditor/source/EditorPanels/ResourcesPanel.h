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

	private:
		std::filesystem::path actualDir;

		std::string search;

		std::unordered_map<std::filesystem::path, UUID> assets;

		std::shared_ptr<Texture2D> folderIcon;
	};
}