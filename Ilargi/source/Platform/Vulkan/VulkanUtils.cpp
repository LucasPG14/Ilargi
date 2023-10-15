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
			case ImageFormat::RED8:	return VK_FORMAT_R8_SRGB;
			case ImageFormat::RED16:	return VK_FORMAT_R16_SFLOAT;
			case ImageFormat::RED32:	return VK_FORMAT_R32_SFLOAT;
			case ImageFormat::RGBA8:	return VK_FORMAT_R8G8B8A8_SRGB;
			case ImageFormat::RGBA16:	return VK_FORMAT_R16G16B16A16_SFLOAT;
			case ImageFormat::RGBA32:	return VK_FORMAT_R32G32B32A32_SFLOAT;
			}

			ILG_ASSERT(false, "Vulkan format not supported");
			return VkFormat();
		}
	}
}