#include "ilargipch.h"

#include "TextureImporter.h"
#include "Utils/FileSystem.h"

#include "Resources/ResourceManager.h"

#include <stb_image.h>
#define STB_DXT_IMPLEMENTATION
#include <stb_dxt.h>

namespace Ilargi
{
	TextureImporterOptions TextureImporter::sOptions = {};

	void TextureImporter::ImportTexture(UUID aUUID, const ResourceMetadata& aMetadata)
	{
		BinaryWriter writter(aMetadata.filepath.string());
		int desiredChannels, channels;
		TextureHeader textureHeader;

		stbi_set_flip_vertically_on_load(true);

		stbi_info(aMetadata.sourceFile.string().c_str(), &textureHeader.width, &textureHeader.height, &desiredChannels);
		desiredChannels = desiredChannels == 3 ? 4 : desiredChannels;
		void* data{ stbi_load(aMetadata.sourceFile.string().c_str(), &textureHeader.width, &textureHeader.height, &channels, desiredChannels) };
		
		textureHeader.channels = desiredChannels;
		if (!data)
		{
			ILG_CORE_ERROR("Unable to load the texture: {0}", aMetadata.sourceFile.string());
			return;
		}
		
		writter.Write(textureHeader);
		writter.Write(data, textureHeader.width * textureHeader.height * textureHeader.channels);

		if (!sOptions.normalMap)
		{
			// TODO: Compress image
		}
	}
	
	std::shared_ptr<Texture2D> TextureImporter::LoadTexture(const ResourceMetadata& aMetadata)
	{
		// TODO: Take a look to the formats, doesn't allow to create an image with three channels
		std::shared_ptr<Texture2D> texture;

		BinaryReader reader(aMetadata.filepath.string());
		
		TextureHeader textureHeader;
		reader.Read(textureHeader);
		
		void* imageData{ new char[textureHeader.width * textureHeader.height * textureHeader.channels] };
		reader.Read(imageData, textureHeader.width * textureHeader.height * textureHeader.channels);

		return Texture2D::Create(imageData, textureHeader.width, textureHeader.height, textureHeader.channels);
	}
}