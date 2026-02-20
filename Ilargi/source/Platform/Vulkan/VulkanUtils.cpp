#include "ilargipch.h"

#include "VulkanUtils.h"

namespace Ilargi
{
	namespace Utils
	{
		const VkFormat GetFormatFromImageFormat(ImageFormat aFormat) noexcept
		{
			switch (aFormat)
			{
			case ImageFormat::RED8:					return VK_FORMAT_R8_SRGB;
			case ImageFormat::RED16:				return VK_FORMAT_R16_SFLOAT;
			case ImageFormat::RED32:				return VK_FORMAT_R32_SFLOAT;
			case ImageFormat::RED32_UINT:			return VK_FORMAT_R32_UINT;
			case ImageFormat::RGBA8:				return VK_FORMAT_R8G8B8A8_SRGB;
			case ImageFormat::RGBA16:				return VK_FORMAT_R16G16B16A16_SFLOAT;
			case ImageFormat::RGBA32:				return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ImageFormat::DEPTH32:				return VK_FORMAT_D32_SFLOAT;
			case ImageFormat::DEPTH24_STENCIL8:		return VK_FORMAT_D24_UNORM_S8_UINT;
			}

			ILG_ASSERT(false, "Vulkan format not supported");
			return VkFormat();
		}

		const bool IsDepth(ImageFormat aFormat) noexcept
		{
			if (aFormat == ImageFormat::DEPTH32 || aFormat == ImageFormat::DEPTH24_STENCIL8)
				return true;

			return false;
		}
		
		const VkDescriptorType GetVulkanDescriptorType(DescriptorType aType) noexcept
		{
			switch (aType)
			{
			case DescriptorType::UNIFORM_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case DescriptorType::COMBINED_IMAGE_SAMPLER: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			}

			return VK_DESCRIPTOR_TYPE_MAX_ENUM;
		}
		
		constexpr DescriptorType GetDescriptorTypeFromVulkan(VkDescriptorType aType) noexcept
		{
			switch (aType)
			{			
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return DescriptorType::UNIFORM_BUFFER;
			case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: return DescriptorType::COMBINED_IMAGE_SAMPLER;
			}

			return DescriptorType();
		}

		constexpr VkShaderStageFlags GetVulkanShaderStage(ShaderStage aShaderStage) noexcept
		{
			switch (aShaderStage)
			{
			case ShaderStage::VERTEX_SHADER: return VK_SHADER_STAGE_VERTEX_BIT;
			case ShaderStage::FRAGMENT_SHADER: return VK_SHADER_STAGE_FRAGMENT_BIT;
			}

			return VK_SHADER_STAGE_VERTEX_BIT;
		}

		const ShaderStage GetShaderStage(VkShaderStageFlags aShaderStage) noexcept
		{
			switch (aShaderStage)
			{
			case VK_SHADER_STAGE_VERTEX_BIT: return VERTEX_SHADER;
			case VK_SHADER_STAGE_FRAGMENT_BIT: return FRAGMENT_SHADER;
			}

			return VERTEX_SHADER;
		}
	}
}