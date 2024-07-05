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
	};

	struct ResourceMetadata
	{
		ResourceType type = ResourceType::NONE;
		std::filesystem::path filepath;
		std::filesystem::path sourceFile;
	};

	class Resource
	{
	public:
		virtual const ResourceType GetType() const = 0;
	};
}