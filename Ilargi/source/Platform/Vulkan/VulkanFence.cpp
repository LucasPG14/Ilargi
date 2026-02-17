#include "ilargipch.h"
#include "VulkanFence.h"

#include "VulkanGraphicsContext.h"

namespace Ilargi
{
	VulkanFence::VulkanFence() : mFence(VK_NULL_HANDLE)
	{
		const VkDevice& device{ VulkanGraphicsContext::GetLogicalDevice() };

		VkFenceCreateInfo fenceInfo
		{
			.sType {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO},
			.pNext {nullptr},
			.flags {VK_FENCE_CREATE_SIGNALED_BIT}
		};

		VK_CHECK_RESULT(vkCreateFence(device, &fenceInfo, nullptr, &mFence));
	}
}