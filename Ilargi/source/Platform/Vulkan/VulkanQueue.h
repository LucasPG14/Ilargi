#pragma once

#include "Renderer/IQueue.h"

#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanQueue : public IQueue
	{
	public:
		VulkanQueue(uint32_t aQueueFamilyIndex);
		virtual ~VulkanQueue();
	private:
		VkQueue mQueue;
	};
}