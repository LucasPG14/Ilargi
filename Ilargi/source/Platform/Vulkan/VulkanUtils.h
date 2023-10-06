#pragma once

#include "Renderer/Framebuffer.h"
#include <vulkan/vulkan.h>

#define VK_CHECK_RESULT(x)		{ if (x != VK_SUCCESS) __debugbreak(); }

namespace Ilargi
{
	namespace Utils
	{
		VkFormat GetFormatFromImageFormat(ImageFormat format);
	}
}