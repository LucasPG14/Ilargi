#pragma once

namespace Ilargi
{
	class StaticMesh;

	class ModelImporter
	{
	public:
		static std::shared_ptr<StaticMesh> ImportModel(const std::string path);
	};
}