#pragma once

#include "Renderer/ISemaphore.h"

#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanSemaphore : public ISemaphore
	{
	public:
		VulkanSemaphore();

		virtual ~VulkanSemaphore();

	private:
		VkSemaphore mSemaphore;
	};
}