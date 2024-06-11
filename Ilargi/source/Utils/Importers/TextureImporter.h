#pragma once

namespace Ilargi
{
	struct TextureImporterOptions
	{
		bool normalMap = false;
	};


	class TextureImporter
	{
	public:
		static void ImportTexture(const std::filesystem::path& dirToSave, const std::filesystem::path& filepath);

	private:
		static TextureImporterOptions options;
	};
}