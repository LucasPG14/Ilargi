#pragma once

#include "Base/UUID.h"

namespace Ilargi
{
	enum class ResourceType
	{
		NONE = 0,
		MODEL,
		MESH,
		TEXTURE2D,
		MATERIAL,
		SCENE,
		SKYBOX
	};

	struct ResourceMetadata
	{
		ResourceType type = ResourceType::NONE;
		std::filesystem::path filepath;
		std::filesystem::path sourceFile;
		std::filesystem::file_time_type lastWriteTime;
	};

	class Resource
	{
	public:
		[[nodiscard]] virtual const ResourceType GetType() const = 0;

	public:
		UUID mResourceUUID;
	};
}