#include "ilargipch.h"

#include "VulkanTexture.h"

namespace Ilargi
{
	VulkanTexture2D::VulkanTexture2D(void* data, uint32_t w, uint32_t h) : width(w), height(h), image()
	{
		VkImageCreateInfo imageInfo = {};
	}
	
	VulkanTexture2D::~VulkanTexture2D()
	{
	}
}