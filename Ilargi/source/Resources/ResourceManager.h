#pragma once

#include "Resource.h"

namespace Ilargi
{
	class ResourceManager
	{
	public:
		/*
		* @brief Clears all the resources.
		*/
		static void Clear();

		/*
		* @brief Registers a new resource by a given resource metadata.
		* @param aMetadata The metadata of the resource.
		* @return The identifier of the resource.
		*/
		[[nodiscard]] static UUID RegisterResource(const ResourceMetadata& aMetadata);
		
		/*
		* @brief Imports a new resource by his source file.
		* @param aActualDir The actual directory of the resources panel.
		* @param aPath The source file of the resource.
		* @return The identifier of the resource.
		*/
		[[nodiscard]] static UUID ImportResource(const std::filesystem::path& aActualDir, const std::filesystem::path& aPath);
		
		/*
		* @brief Reloads a resource.
		* @param aUUID The resource identifier.
		* @return An instance of the resource.
		*/
		[[nodiscard]] static std::shared_ptr<Resource> ReloadResource(UUID aUUID);

		/*
		* @brief Saves the resource when it has been modified on the engine(materials...).
		* @param aResource The resource to save.
		*/
		static void SaveResource(const std::shared_ptr<Resource>& aResource);

		/*
		* @brief Checks if a resource exists.
		* @param aUUID The resource identifier.
		* @return True if exists, false otherwise.
		*/
		[[nodiscard]] static bool ExistsResource(UUID aUUID);

		/*
		* @brief Checks if a resource is already loaded.
		* @param aUUID The resource identifier.
		* @return True if its loaded, false otherwise.
		*/
		[[nodiscard]] static bool IsResourceLoaded(UUID aUUID);

		/*
		* @brief Returns the metadata of a resource.
		* @param aUUID The resource identifier.
		* @return The resource metadata.
		*/
		[[nodiscard]] static const ResourceMetadata& GetMetadata(UUID aUUID);

		/*
		* @brief Returns the resource.
		* @param aUUID The resource identifier.
		* @return Instance of the resource.
		*/
		[[nodiscard]] static std::shared_ptr<Resource> GetResource(UUID aUUID);

		/*
		* @brief Remove the resource of the given identifier.
		* @param aUUID The resource identifier.
		*/
		static void RemoveResource(UUID aUUID);

		/*
		* @brief Loads the information of resources of the project from a json file.
		*/
		static void LoadResourceRegistry();

		/*
		* @brief Saves the information of resources of the project to a json file.
		*/
		static void SaveResourceRegistry();

		/*
		* @brief Returns the map of resource metadatas.
		* @return The resource metadatas map.
		*/
		[[nodiscard]] static std::unordered_map<UUID, ResourceMetadata> GetResourcesMetadata() { return sResourcesMetadata; }
		
		/*
		* @brief Returns the map of loaded resources.
		* @return The loaded resources map.
		*/
		[[nodiscard]] static std::unordered_map<UUID, std::shared_ptr<Resource>> GetLoadedResources() { return sLoadedResources; }

	private:
		/*
		* @brief Returns the resource type from a string.
		* @param aStringType The extension of the file.
		* @return The resource type.
		*/
		[[nodiscard]] static const ResourceType GetResourceType(const std::string& aStringType);

	private:
		static std::unordered_map<UUID, ResourceMetadata> sResourcesMetadata; // The metadatas map.
		static std::unordered_map<UUID, std::shared_ptr<Resource>> sLoadedResources; // The loaded resources map.
	};
}