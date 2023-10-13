#pragma once

#include "Renderer/Texture.h"
#include "Platform/Vulkan/VulkanAllocator.h"

namespace Ilargi
{
	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(void* data, uint32_t w, uint32_t h);
		virtual ~VulkanTexture2D();

		const uint32_t GetWidth() const override { return width; }
		const uint32_t GetHeight() const override { return height; }

		const void* GetID() const override { return nullptr; }

	private:
		uint32_t width;
		uint32_t height;

		Image image;
	};
}