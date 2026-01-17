#include "ilargipch.h"
#include "TextureThumbnail.h"

#include "Resources/ResourceManager.h"
#include "Resources/Texture.h"
#include "Utils/FileSystem.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_resize2.h>

namespace Ilargi
{
	constexpr int THUMBNAIL_SIZE{ 256 };

	namespace Utils
	{
		stbir_pixel_layout GetPixelLayoutFromChannels(int aChannels)
		{
			switch (aChannels)
			{
			case 1: return STBIR_1CHANNEL;
			case 2: return STBIR_2CHANNEL;
			case 4: return STBIR_4CHANNEL;
			}

			return STBIR_4CHANNEL;
		}
	}

	std::shared_ptr<Texture2D> TextureThumbnail::Create(UUID aUUID, const ResourceMetadata& aMetadata)
	{
		const std::string thumbnailPath{ std::format("Engine/Thumbnails/{}.thumb", static_cast<uint64_t>(aUUID)) };

		if (std::filesystem::exists(thumbnailPath))
		{
			BinaryReader reader(thumbnailPath);

			int channels;
			reader.Read(channels);
			unsigned char* data{ new unsigned char[THUMBNAIL_SIZE * THUMBNAIL_SIZE * channels] };
			reader.Read(data, THUMBNAIL_SIZE * THUMBNAIL_SIZE * channels);

			return Texture2D::Create(data, THUMBNAIL_SIZE, THUMBNAIL_SIZE, channels);
		}

		int width, height, channels, desiredChannels;
		stbi_info(aMetadata.sourceFile.string().c_str(), &width, &height, &desiredChannels);
		desiredChannels = desiredChannels == 3 ? 4 : desiredChannels;

		unsigned char* data { stbi_load(aMetadata.sourceFile.string().c_str(), &width, &height, &channels, desiredChannels) };

		unsigned char* resizedData{ stbir_resize_uint8_srgb(data, width, height, 0, nullptr, THUMBNAIL_SIZE, THUMBNAIL_SIZE, 0, Utils::GetPixelLayoutFromChannels(desiredChannels)) };

		BinaryWriter writter(thumbnailPath);
		writter.Write(4);
		writter.Write(resizedData, THUMBNAIL_SIZE * THUMBNAIL_SIZE * desiredChannels);

		return Texture2D::Create(resizedData, THUMBNAIL_SIZE, THUMBNAIL_SIZE, desiredChannels);
	}
}