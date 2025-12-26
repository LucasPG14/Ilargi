#pragma once

#include "Resources/Skybox.h"
#include "VulkanAllocator.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanSkybox : public Skybox
	{
	public:
		VulkanSkybox(std::shared_ptr<Shader> aShader);
		~VulkanSkybox();

		const void* GetDescriptorSet() const override { return mDescriptorSet; }

	private:
		VkDescriptorSet mDescriptorSet;
		uint32_t mWidth;
		uint32_t mHeight;

		Image mImage;
		VkImageView mImageView;
		VkSampler mSampler;
	};
}