#include "ilargipch.h"

#include "TextureImporter.h"
#include "Utils/FileSystem.h"

#include "Resources/ResourceManager.h"

#include <stb_image.h>
#define STB_DXT_IMPLEMENTATION
#include <stb_dxt.h>

namespace Ilargi
{
	TextureImporterOptions TextureImporter::options = {};

	void TextureImporter::ImportTexture(const std::filesystem::path& dirToSave, const std::filesystem::path& filepath)
	{
		Buffer buffer;
		int width, height, channels;

		// TODO: Need to save the width, height and channels of the image in the binary file
		buffer.data = stbi_load(filepath.string().c_str(), &width, &height, &channels, 0);

		if (!buffer.data)
		{
			ILG_CORE_ERROR("Unable to load the texture: {0}", filepath.string());
			return;
		}

		buffer.size = width * height * channels;

		if (!options.normalMap)
		{
			// TODO: Compress image
		}

		auto s = dirToSave;
		s += filepath.stem();
		s += ".ires";

		FileSystem::WriteBinaryFile(s, buffer);

		ResourceMetadata metadata;
		metadata.type = ResourceType::TEXTURE2D;
		metadata.sourceFile = filepath;
		metadata.filepath = s;

		ResourceManager::RegisterResource(metadata);
	}
}