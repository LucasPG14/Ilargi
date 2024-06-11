#pragma once

#include "Events/Event.h"
#include "Events/WindowEvents.h"

namespace Ilargi
{
	class UUID;

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

		std::unordered_map<std::filesystem::path, UUID> assets;
	};
}