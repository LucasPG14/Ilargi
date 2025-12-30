#pragma once

#include "Resource.h"

namespace Ilargi
{
	class ResourceManager
	{
	public:
		static void Clear();

		[[nodiscard]] static UUID RegisterResource(const ResourceMetadata& aMetadata);
		[[nodiscard]] static UUID ImportResource(const std::filesystem::path& aActualDir, const std::filesystem::path& aPath);
		
		[[nodiscard]] static std::shared_ptr<Resource> ReloadResource(UUID aUUID);
		static void SaveResource(const std::shared_ptr<Resource>& aResource);

		[[nodiscard]] static bool ExistsResource(UUID aUUID);
		[[nodiscard]] static bool IsResourceLoaded(UUID aUUID);

		[[nodiscard]] static const ResourceMetadata& GetMetadata(UUID aUUID);
		[[nodiscard]] static std::shared_ptr<Resource> GetResource(UUID aUUID);

		static void RemoveResource(UUID aUUID);

		static void LoadResourceRegistry();
		static void SaveResourceRegistry();

		[[nodiscard]] static std::unordered_map<UUID, ResourceMetadata> GetResourcesMetadata() { return sResourcesMetadata; }
		[[nodiscard]] static std::unordered_map<UUID, std::shared_ptr<Resource>> GetLoadedResources() { return sLoadedResources; }

	private:
		[[nodiscard]] static const ResourceType GetResourceType(const std::string& mStringType);

	private:
		static std::unordered_map<UUID, ResourceMetadata> sResourcesMetadata;
		static std::unordered_map<UUID, std::shared_ptr<Resource>> sLoadedResources;
	};
}