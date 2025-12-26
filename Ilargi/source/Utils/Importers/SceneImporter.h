#pragma once

#include "Base/UUID.h"
#include "Resources/Resource.h"

namespace Ilargi
{
	class Scene;

	class SceneImporter
	{
	public:
		static void ImportScene(UUID aUUID, const ResourceMetadata& aMetadata);
		static std::shared_ptr<Resource> LoadScene(const ResourceMetadata& aMetadata);
		static void SaveScene(const std::shared_ptr<Scene>& aScene, const std::filesystem::path& aFilepath);
	};
}