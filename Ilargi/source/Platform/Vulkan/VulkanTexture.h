#pragma once

#include "Resources/Texture.h"
#include "Platform/Vulkan/VulkanAllocator.h"

namespace Ilargi
{
	class VulkanTexture2D : public Texture2D
	{
	public:
		/*
		* @brief Constructor.
		* @param aFilepath The path of the texture file.
		*/
		VulkanTexture2D(std::filesystem::path aFilepath);

		/*
		* @brief Constructor.
		* @param aData The texture data.
		* @param aWidth The texture width.
		* @param aHeight The texture height.
		* @param aChannels The number of channels of the texture.
		*/
		VulkanTexture2D(void* aData, int aWidth, int aHeight, int aChannels);
		
		/*
		* @brief Destructor.
		*/
		virtual ~VulkanTexture2D();

		/*
		* @copydoc Texture2D::GetWidth()
		*/
		[[nodiscard]] const uint32_t GetWidth() const override { return mWidth; }

		/*
		* @copydoc Texture2D::GetHeight()
		*/
		[[nodiscard]] const uint32_t GetHeight() const override { return mHeight; }

		/*
		* @copydoc Texture2D::GetID()
		*/
		[[nodiscard]] const void* GetID() const override { return mDescriptorSet; }

		/*
		* @brief Returns the image view of the texture.
		* @return The image view of the texture.
		*/
		[[nodiscard]] const VkImageView GetImageView() const { return mImageView; }

		/*
		* @brief Returns the sampler of the texture.
		* @return The sampler of the texture.
		*/
		[[nodiscard]] const VkSampler GetSampler() const { return mSampler; }

	private:
		/*
		* @brief Transitions the texture from a specified old layout to the new.
		* @param aMipLevels The number of mip map levels.
		* @param aOldLayout The actual layout of the texture.
		* @param aNewLayout The new layout of the texture.
		*/
		void TransitionLayout(uint32_t aMipLevels, VkImageLayout aOldLayout, VkImageLayout aNewLayout);

		/*
		* @brief Generates the mip map levels of the texture.
		* @param aMipLevels The number of mip map levels.
		*/
		void GenerateMipMaps(uint32_t aMipLevels);
	private:
		Image mImage; // Vulkan image and allocation.
		VkImageView mImageView; // Image view of the texture.
		VkSampler mSampler; // Sampler of the texture
		VkDescriptorSet mDescriptorSet; // Descriptor set of the texture.

		uint32_t mWidth; // Width of the texture.
		uint32_t mHeight; // Height of the texture.
	};
}