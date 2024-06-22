#pragma once

#include "Scene.h"

namespace Ilargi
{
	class SceneLoaderSaver
	{
	public:
		SceneLoaderSaver(const std::shared_ptr<Scene>& scn);
		~SceneLoaderSaver();

		void LoadScene(const std::filesystem::path& path);
		void SaveScene(const std::filesystem::path& path);

	private:
		std::shared_ptr<Scene> scene;
	};
}