#include "ilargipch.h"

#include "VulkanUtils.h"

namespace Ilargi
{
	namespace Utils
	{
		VkFormat GetFormatFromImageFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGBA_8:	return VK_FORMAT_R8G8B8A8_SRGB;
			case ImageFormat::RGBA_16:	return VK_FORMAT_R16G16B16A16_SFLOAT;
			}

			ILG_ASSERT(false, "Format not supported");
			return VkFormat();
		}
	}
}