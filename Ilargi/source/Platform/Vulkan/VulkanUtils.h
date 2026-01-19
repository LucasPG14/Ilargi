#pragma once

#include "Renderer/Framebuffer.h"
#include "Renderer/DescriptorSetLayout.h"
#include <vulkan/vulkan.h>

#define VK_CHECK_RESULT(x)		{ if (x != VK_SUCCESS) __debugbreak(); }

namespace Ilargi
{
	namespace Utils
	{
		/*
		* @brief Gets the vulkan format from the Ilargi::ImageFormat.
		* @param aFormat The format of the image.
		* @return The vulkan format.
		*/
		[[nodiscard]] const VkFormat GetFormatFromImageFormat(ImageFormat aFormat);

		/*
		* @brief Returns if the format passed is depth or not.
		* @param aFormat The format of the image.
		* @return True if is depth, false otherwise.
		*/
		bool IsDepth(ImageFormat aFormat);

		/*
		* @brief Returns the descriptor type as VkDescriptorType.
		* @param aType The type of the descriptor.
		* @return The type of vulkan descriptor.
		*/
		VkDescriptorType GetVulkanDescriptorType(DescriptorType aType);

		/*
		* @brief Returns the descriptor type from VkDescriptorType.
		* @param aType The vulkan descriptor type.
		* @return The type of descriptor.
		*/
		DescriptorType GetDescriptorTypeFromVulkan(VkDescriptorType aType);
	}
}