#pragma once

#include "Resource.h"

namespace Ilargi
{
	class ResourceManager
	{
	public:
		static void Clear();

		static UUID RegisterResource(const ResourceMetadata& metadata);
		static UUID ImportResource(const std::filesystem::path& actualDir, const std::filesystem::path& path);
		static std::shared_ptr<Resource> LoadResource(const ResourceMetadata& metadata);

		static bool ExistsResource(UUID uuid);

		static const ResourceMetadata& GetMetadata(UUID uuid);
		static std::shared_ptr<Resource> GetResource(UUID uuid);

		static bool IsResourceLoaded(UUID uuid);

		static std::unordered_map<UUID, ResourceMetadata> GetResourcesMetadata() { return resourcesMetadata; }
		static std::unordered_map<UUID, std::shared_ptr<Resource>> GetLoadedResources() { return loadedResources; }

		static void RemoveResource(UUID uuid);

		static void SaveResourceRegistry();
		static void LoadResourceRegistry();
	private:
		static const ResourceType GetResourceType(const std::string& str);

	private:
		static std::unordered_map<UUID, ResourceMetadata> resourcesMetadata;
		static std::unordered_map<UUID, std::shared_ptr<Resource>> loadedResources;
	};
}