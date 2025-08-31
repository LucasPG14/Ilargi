#pragma once

#include "Resources/Resource.h"
#include "Resources/Texture.h"

namespace Ilargi
{
	struct TextureImporterOptions
	{
		bool normalMap{ false };
	};

	class UUID;

	class TextureImporter
	{
	public:
		static void ImportTexture(UUID aUUID, const ResourceMetadata& aMetadata);

		static std::shared_ptr<Texture2D> LoadTexture(const ResourceMetadata& aMetadata);
	private:
		static TextureImporterOptions sOptions;
	};
}