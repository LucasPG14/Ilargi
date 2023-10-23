#pragma once

#include "Resources/Texture.h"
#include "Platform/Vulkan/VulkanAllocator.h"

namespace Ilargi
{
	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(std::filesystem::path filepath);
		virtual ~VulkanTexture2D();

		const uint32_t GetWidth() const override { return width; }
		const uint32_t GetHeight() const override { return height; }

		const void* GetID() const override { return descriptorSet; }

		const VkImageView GetImageView() const { return imageView; }
		const VkSampler GetSampler() const { return sampler; }

	private:
		void TransitionLayout(uint32_t mipLevels, VkImageLayout oldLayout, VkImageLayout newLayout);

		void GenerateMipMaps(uint32_t mipLevels);
	private:
		uint32_t width;
		uint32_t height;

		Image image;
		VkImageView imageView;
		VkSampler sampler;
		VkDescriptorSet descriptorSet;
	};
}