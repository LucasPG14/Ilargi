#include "ilargipch.h"
#include "VulkanQueue.h"

#include "VulkanGraphicsContext.h"

namespace Ilargi
{
	VulkanQueue::VulkanQueue(uint32_t aQueueFamilyIndex) : mQueue(VK_NULL_HANDLE)
	{
		const VkDevice& device{ VulkanGraphicsContext::GetLogicalDevice() };
		vkGetDeviceQueue(device, aQueueFamilyIndex, 0, &mQueue);
	}
	
	VulkanQueue::~VulkanQueue()
	{
	}
}