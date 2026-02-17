#include "ilargipch.h"
#include "VulkanSemaphore.h"

#include "VulkanGraphicsContext.h"

namespace Ilargi
{
	VulkanSemaphore::VulkanSemaphore() : mSemaphore(VK_NULL_HANDLE)
	{
		auto device{ VulkanGraphicsContext::GetLogicalDevice() };

		VkSemaphoreCreateInfo semaphoreInfo
		{
			.sType {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO},
			.pNext {nullptr},
			.flags {0U}
		};

		VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &mSemaphore));
	}
}