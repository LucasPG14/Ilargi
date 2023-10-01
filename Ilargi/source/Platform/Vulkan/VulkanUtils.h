#pragma once

#include "Renderer/Framebuffer.h"
#include <vulkan/vulkan.h>

#define VK_CHECK_RESULT(x, ...)		{ if(!x) {	ILG_ASSERT(x, __VA_ARGS__); } }

namespace Ilargi
{
	namespace Utils
	{
		VkFormat GetFormatFromImageFormat(ImageFormat format);
	}
}