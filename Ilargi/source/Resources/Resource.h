#pragma once

#include "Base/UUID.h"

namespace Ilargi
{
	enum class ResourceType
	{
		NONE = 0,
		MESH,
		TEXTURE,
		MATERIAL
		//SCENE,
	};

	struct ResourceMetaData
	{
		UUID resourceID;
		std::filesystem::path filepath;
	};

	class Resource
	{
	public:
		virtual const ResourceType GetType() const = 0;
	};
}