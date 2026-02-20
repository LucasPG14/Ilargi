#pragma once

#include "Renderer/IFramebuffer.h"
#include "Renderer/IDescriptorSetLayout.h"
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
		[[nodiscard]] const VkFormat GetFormatFromImageFormat(ImageFormat aFormat) noexcept;

		/*
		* @brief Returns if the format passed is depth or not.
		* @param aFormat The format of the image.
		* @return True if is depth, false otherwise.
		*/
		[[nodiscard]] const bool IsDepth(ImageFormat aFormat) noexcept;

		/*
		* @brief Returns the descriptor type as VkDescriptorType.
		* @param aType The type of the descriptor.
		* @return The type of vulkan descriptor.
		*/
		[[nodiscard]] const VkDescriptorType GetVulkanDescriptorType(DescriptorType aType) noexcept;

		/*
		* @brief Returns the descriptor type from VkDescriptorType.
		* @param aType The vulkan descriptor type.
		* @return The type of descriptor.
		*/
		[[nodiscard]] constexpr DescriptorType GetDescriptorTypeFromVulkan(VkDescriptorType aType) noexcept;

		/*
		* @brief Returns the vulkan shader stage from ShaderStage.
		* @param aShaderStage The shader stage type.
		* @return The vulkan shader stage.
		*/
		[[nodiscard]] constexpr VkShaderStageFlags GetVulkanShaderStage(ShaderStage aShaderStage) noexcept;

		/*
		* @brief Returns the shader stage from VkShaderStageFlags.
		* @param aShaderStage The vulkan shader stage.
		* @return The shader stage.
		*/
		[[nodiscard]] const ShaderStage GetShaderStage(VkShaderStageFlags aShaderStage) noexcept;
	}
}