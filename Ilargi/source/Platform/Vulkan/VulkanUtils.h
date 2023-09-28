#pragma once

#include "Renderer/Framebuffer.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	namespace Utils
	{
		VkFormat GetFormatFromImageFormat(ImageFormat format);
	}
}