#pragma once

#include "Resources/Resource.h"
#include "Resources/Texture.h"

namespace Ilargi
{
	struct TextureImporterOptions
	{
		bool normalMap{ false };
	};

	struct TextureHeader
	{
		uint32_t magic = 0x54585452;
		uint32_t version = 1;
		int width;
		int height;
		uint8_t channels;
	};

	class UUID;

	class TextureImporter
	{
	public:
		/*
		* @brief Imports a texture.
		* @param aUUID The identifier of the texture.
		* @param aMetadata The metadata of the texture.
		*/
		static void ImportTexture(UUID aUUID, const ResourceMetadata& aMetadata);

		/*
		* @brief Loads a texture.
		* @param aMetadata The metadata of the texture.
		* @return An instance of the texture loaded.
		*/
		static std::shared_ptr<Texture2D> LoadTexture(const ResourceMetadata& aMetadata);
	private:
		static TextureImporterOptions sOptions; // Texture Options for import
	};
}