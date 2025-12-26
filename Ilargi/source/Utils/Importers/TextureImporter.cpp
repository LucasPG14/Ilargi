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
		Buffer buffer;
		int width, height, channels;

		stbi_set_flip_vertically_on_load(true);

		void* data{ stbi_load(aMetadata.sourceFile.string().c_str(), &width, &height, &channels, STBI_rgb_alpha) };

		if (!data)
		{
			ILG_CORE_ERROR("Unable to load the texture: {0}", aMetadata.sourceFile.string());
			return;
		}
		int header[3] { width, height, 4 };
		buffer.size = sizeof(header) + (width * height * 4);

		buffer.data = new char[buffer.size];
		
		char* buf{ buffer.data };
		memcpy(buf, header, sizeof(header));
		buf += sizeof(header);
		memcpy(buf, data, width * height * 4);

		if (!sOptions.normalMap)
		{
			// TODO: Compress image
		}

		FileSystem::WriteBinaryFile(aMetadata.filepath, buffer);
	}
	
	std::shared_ptr<Texture2D> TextureImporter::LoadTexture(const ResourceMetadata& aMetadata)
	{
		// TODO: Take a look to the formats, doesn't allow to create an image with three channels
		std::shared_ptr<Texture2D> texture;

		const Buffer& buffer{ FileSystem::ReadBinaryFile(aMetadata.filepath) };

		char* data{ buffer.data };

		int width, height, channels;

		memcpy(&width, data, sizeof(int));
		data += sizeof(int);

		memcpy(&height, data, sizeof(int));
		data += sizeof(int);

		memcpy(&channels, data, sizeof(int));
		data += sizeof(int);

		void* imageData{ new char[width * height * channels] };
		memcpy(imageData, data, width * height * channels);

		return Texture2D::Create(imageData, width, height, channels);
	}
}