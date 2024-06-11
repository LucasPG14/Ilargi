#pragma once

namespace Ilargi
{
	class StaticMesh;

	class ModelImporter
	{
	public:
		static std::shared_ptr<StaticMesh> ImportModel(const std::string path);

		static void ImportModel2(const std::filesystem::path& path, const std::filesystem::path& assetsPath);
	};
}