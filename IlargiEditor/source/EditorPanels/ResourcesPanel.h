#pragma once

namespace Ilargi
{
	class ResourcesPanel
	{
	public:
		ResourcesPanel();
		~ResourcesPanel();

		void Render();

	private:
		std::filesystem::path actualDir;
	};
}