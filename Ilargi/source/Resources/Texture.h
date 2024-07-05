#pragma once

#include "Resources/Resource.h"

namespace Ilargi
{
	class Texture2D : public Resource
	{
	public:
		virtual const uint32_t GetWidth() const = 0;
		virtual const uint32_t GetHeight() const = 0;

		virtual const void* GetID() const = 0;

		static ResourceType GetStaticType() { return ResourceType::TEXTURE2D; }
		const ResourceType GetType() const { return GetStaticType(); }

		virtual const void Destroy() = 0;
		static std::shared_ptr<Texture2D> Create(std::filesystem::path filepath);
	};
}