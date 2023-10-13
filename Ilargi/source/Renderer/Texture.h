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

		const ResourceType GetType() const { return ResourceType::NONE; }

		static std::shared_ptr<Texture2D> Create(void* data, uint32_t width, uint32_t height);
	};
}