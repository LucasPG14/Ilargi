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

	void TextureImporter::ImportTexture(UUID uuid, const ResourceMetadata& metadata)
	{
		Buffer buffer;
		int width, height, channels;

		stbi_set_flip_vertically_on_load(true);

		// TODO: Need to save the width, height and channels of the image in the binary file
		void* data = stbi_load(metadata.sourceFile.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

		if (!data)
		{
			ILG_CORE_ERROR("Unable to load the texture: {0}", metadata.sourceFile.string());
			return;
		}
		int header[3] = { width, height, 4 };
		buffer.size = sizeof(header) + (width * height * 4);

		buffer.data = new char[buffer.size];
		
		char* buf = buffer.data;
		memcpy(buf, header, sizeof(header));
		buf += sizeof(header);
		memcpy(buf, data, width * height * 4);

		if (!sOptions.normalMap)
		{
			// TODO: Compress image
		}

		FileSystem::WriteBinaryFile(metadata.filepath, buffer);
	}
	
	std::shared_ptr<Texture2D> TextureImporter::LoadTexture(const ResourceMetadata& metadata)
	{
		// TODO: Take a look to the formats, doesn't allow to create an image with three channels
		std::shared_ptr<Texture2D> texture;

		const Buffer& buffer = FileSystem::ReadBinaryFile(metadata.filepath);

		char* data = buffer.data;

		int width, height, channels;

		memcpy(&width, data, sizeof(int));
		data += sizeof(int);

		memcpy(&height, data, sizeof(int));
		data += sizeof(int);

		memcpy(&channels, data, sizeof(int));
		data += sizeof(int);

		void* imageData = new char[width * height * channels];
		memcpy(imageData, data, width * height * channels);

		return Texture2D::Create(imageData, width, height, channels);
	}
}