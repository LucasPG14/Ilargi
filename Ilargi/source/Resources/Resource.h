#pragma once

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

	class Resource
	{
	public:
		virtual const ResourceType GetType() const = 0;
	};
}