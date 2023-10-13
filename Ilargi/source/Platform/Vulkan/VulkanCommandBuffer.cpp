#include "ilargipch.h"

#include "VulkanCommandBuffer.h"
#include "VulkanContext.h"
#include "Renderer/Renderer.h"

namespace Ilargi
{
	VulkanCommandBuffer::VulkanCommandBuffer(uint32_t framesInFlight) : queryPoolCount(framesInFlight * 2)
	{
		auto device = VulkanContext::GetLogicalDevice();

		commandBuffers.resize(framesInFlight);

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = VulkanContext::GetCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()));

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VK_CHECK_RESULT(vkCreateFence(device, &fenceInfo, nullptr, &fence));
	}
	
	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
	}

	void VulkanCommandBuffer::BeginCommand() const
	{
		Renderer::Submit([this]()
			{
				uint32_t currentFrame = Renderer::GetCurrentFrame();

				VkCommandBufferBeginInfo beginInfo{};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = 0; // Optional
				beginInfo.pInheritanceInfo = nullptr; // Optional

				VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo));
			});
	}
	
	void VulkanCommandBuffer::EndCommand() const
	{
		Renderer::Submit([this]()
			{
				uint32_t currentFrame = Renderer::GetCurrentFrame();

				VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffers[currentFrame]));
			});
	}
	
	void VulkanCommandBuffer::Submit() const
	{
		Renderer::Submit([this]()
			{
				uint32_t currentFrame = Renderer::GetCurrentFrame();
				auto device = VulkanContext::GetLogicalDevice();
				
				VkSubmitInfo submitInfo{};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

				VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
				submitInfo.pWaitDstStageMask = waitStages;
				submitInfo.waitSemaphoreCount = 0;
				submitInfo.pWaitSemaphores = nullptr;
				submitInfo.signalSemaphoreCount = 0;
				submitInfo.pSignalSemaphores = nullptr;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

				VK_CHECK_RESULT(vkResetFences(device, 1, &fence));
				VK_CHECK_RESULT(vkQueueSubmit(VulkanContext::GetGraphicsQueue(), 1, &submitInfo, fence));
				
				if (vkGetFenceStatus(device, fence) != VK_SUCCESS)
				{
					vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
				}
			});
	}
}