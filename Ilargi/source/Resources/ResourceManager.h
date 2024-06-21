#pragma once

#include "Resource.h"

namespace Ilargi
{
	class ResourceManager
	{
	public:
		static UUID RegisterResource(const ResourceMetadata& metadata);
		static UUID ImportResource(const std::filesystem::path& path);
		static void LoadResource(UUID uuid);

		static bool HasLoadedResource(UUID uuid);

		static std::unordered_map<UUID, ResourceMetadata> GetResourcesMetadata() { return resourcesMetadata; }
		static std::unordered_map<UUID, std::shared_ptr<Resource>> GetLoadedResources() { return loadedResources; }

		static void SaveResourceRegistry();
		static void LoadResourceRegistry();
	private:
		static const ResourceType GetResourceType(const std::string& str);

	private:
		static std::unordered_map<UUID, ResourceMetadata> resourcesMetadata;
		static std::unordered_map<UUID, std::shared_ptr<Resource>> loadedResources;
	};
}