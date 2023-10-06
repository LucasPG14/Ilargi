#pragma once

namespace Ilargi
{
	enum class ResourceType
	{
		NONE = 0,
		MESH,
		//SCENE,
		//TEXTURE,
		//MATERIAL
	};

	class Resource
	{
	public:
		virtual const ResourceType GetType() const = 0;
	};
}