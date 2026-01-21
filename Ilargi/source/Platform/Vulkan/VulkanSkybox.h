#pragma once

#include "Resources/Skybox.h"
#include "VulkanAllocator.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanSkybox : public Skybox
	{
	public:
		/*
		* @brief Constructor.
		* @param aShader Instance of the skybox shader.
		*/
		VulkanSkybox(const std::shared_ptr<Shader>& aShader);

		/*
		* @brief Destructor.
		*/
		virtual ~VulkanSkybox();

		/*
		* @brief Returns the descriptor set of the Skybox.
		*/
		[[nodiscard]] const void* GetDescriptorSet() const override { return mDescriptorSet; }

	private:
		Image mImage; // Vulkan image and allocation.
		VkDescriptorSet mDescriptorSet; // Instance of the descriptor set.
		VkImageView mImageView; // Image view of the skybox.
		VkSampler mSampler; // Sampler of the skybox.
		uint32_t mWidth; // Width of the skybox.
		uint32_t mHeight; // Height of the skybox.
	};
}