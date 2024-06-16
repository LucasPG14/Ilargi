#pragma once

#include "Resource.h"

namespace Ilargi
{
	class ResourceManager
	{
	public:
		static UUID ImportResource(const std::filesystem::path& path);
		static void LoadAsset(UUID uuid);

		static bool HasLoadedAsset(UUID uuid);

		static std::unordered_map<UUID, ResourceMetaData> GetResourcesMetadata() { return resourcesMetadata; }
		static std::unordered_map<UUID, std::shared_ptr<Resource>> GetLoadedResources() { return loadedResources; }

	private:
		static const ResourceType GetResourceType(const std::string& str);

	private:
		static std::unordered_map<UUID, ResourceMetaData> resourcesMetadata;
		static std::unordered_map<UUID, std::shared_ptr<Resource>> loadedResources;
	};
}