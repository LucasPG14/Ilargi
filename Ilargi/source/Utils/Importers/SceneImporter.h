#pragma once

#include "Base/UUID.h"
#include "Resources/Resource.h"

namespace Ilargi
{
	class Scene;

	class SceneImporter
	{
	public:
		/*
		* @brief Imports a Ilargi scene.
		* @param aUUID The identifier of the scene.
		* @param aMetadata The metadata of the scene.
		*/
		static void ImportScene(UUID aUUID, const ResourceMetadata& aMetadata);

		/*
		* @brief Loads a Ilargi scene.
		* @param aMetadata The metadata of the scene.
		* @return An instance of the scene loaded.
		*/
		static std::shared_ptr<Resource> LoadScene(const ResourceMetadata& aMetadata);

		/*
		* @brief Saves a Ilargi scene.
		* @param aScene The scene to save.
		* @param aFilepath The filepath to save the scene.
		*/
		static void SaveScene(const std::shared_ptr<Scene>& aScene, const std::filesystem::path& aFilepath);
	};
}