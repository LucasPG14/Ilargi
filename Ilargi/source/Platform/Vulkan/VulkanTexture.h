#pragma once

#include "Resources/Texture.h"
#include "Platform/Vulkan/VulkanAllocator.h"

namespace Ilargi
{
	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(std::filesystem::path aFilepath);
		VulkanTexture2D(void* aData, int aWidth, int aHeight, int aChannels);
		virtual ~VulkanTexture2D();

		const uint32_t GetWidth() const override { return mWidth; }
		const uint32_t GetHeight() const override { return mHeight; }

		const void* GetID() const override { return mDescriptorSet; }

		const VkImageView GetImageView() const { return mImageView; }
		const VkSampler GetSampler() const { return mSampler; }

	private:
		void TransitionLayout(uint32_t aMipLevels, VkImageLayout aOldLayout, VkImageLayout aNewLayout);

		void GenerateMipMaps(uint32_t aMipLevels);
	private:
		uint32_t mWidth;
		uint32_t mHeight;

		Image image;
		VkImageView mImageView;
		VkSampler mSampler;
		VkDescriptorSet mDescriptorSet;
	};
}