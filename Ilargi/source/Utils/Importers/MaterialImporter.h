#pragma once

#include "Resources/Resource.h"

namespace Ilargi
{
	class Material;

	class MaterialImporter
	{
	public:
		/*
		* @brief Loads the material.
		* @param aMetadata The metadata of the material.
		* @return An instance of the material loaded.
		*/
		static std::shared_ptr<Resource> LoadMaterial(const ResourceMetadata& aMetadata);
		
		/*
		* @brief Saves the given material.
		* @param aMetadata The metadata of the material.
		* @param aResource An instance of the material to save.
		*/
		static void SaveMaterial(const ResourceMetadata& aMetadata, const std::shared_ptr<Resource>& aResource);
	};
}