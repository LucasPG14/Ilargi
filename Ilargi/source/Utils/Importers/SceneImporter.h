#pragma once

#include "Base/UUID.h"
#include "Resources/Resource.h"

namespace Ilargi
{
	class Scene;

	class SceneImporter
	{
	public:
		static void ImportScene(UUID uuid, const ResourceMetadata& metadata);
		static std::shared_ptr<Resource> LoadScene(const ResourceMetadata& metadata);
		static void SaveScene(std::shared_ptr<Scene> scene, const std::filesystem::path& path);
	};
}