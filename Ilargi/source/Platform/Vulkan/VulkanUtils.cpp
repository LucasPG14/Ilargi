#include "ilargipch.h"

#include "VulkanUtils.h"

namespace Ilargi
{
	namespace Utils
	{
		const VkFormat GetFormatFromImageFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RED8:					return VK_FORMAT_R8_SRGB;
			case ImageFormat::RED16:				return VK_FORMAT_R16_SFLOAT;
			case ImageFormat::RED32:				return VK_FORMAT_R32_SFLOAT;
			case ImageFormat::RGBA8:				return VK_FORMAT_R8G8B8A8_SRGB;
			case ImageFormat::RGBA16:				return VK_FORMAT_R16G16B16A16_SFLOAT;
			case ImageFormat::RGBA32:				return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ImageFormat::DEPTH32:				return VK_FORMAT_D32_SFLOAT;
			case ImageFormat::DEPTH24_STENCIL8:		return VK_FORMAT_D24_UNORM_S8_UINT;
			}

			ILG_ASSERT(false, "Vulkan format not supported");
			return VkFormat();
		}

		bool IsDepth(ImageFormat format)
		{
			if (format == ImageFormat::DEPTH32 || format == ImageFormat::DEPTH24_STENCIL8)
				return true;

			return false;
		}
	}
}