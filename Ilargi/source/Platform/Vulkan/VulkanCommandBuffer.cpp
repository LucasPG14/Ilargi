#include "ilargipch.h"

#include "VulkanCommandBuffer.h"
#include "VulkanContext.h"
#include "Renderer/Renderer.h"

namespace Ilargi
{
	VulkanCommandBuffer::VulkanCommandBuffer(uint32_t aFramesInFlight) : mQueryPoolCount(aFramesInFlight * 2)
	{
		auto device = VulkanContext::GetLogicalDevice();

		mCommandBuffers.resize(aFramesInFlight);

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = VulkanContext::GetCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());

		VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &allocInfo, mCommandBuffers.data()));

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VK_CHECK_RESULT(vkCreateFence(device, &fenceInfo, nullptr, &mFence));
	}
	
	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
	}

	void VulkanCommandBuffer::Destroy() const
	{
		auto device = VulkanContext::GetLogicalDevice();

		vkDestroyFence(device, mFence, nullptr);
	}

	void VulkanCommandBuffer::BeginCommand() const
	{
		Renderer::Submit([this]()
			{
				uint32_t currentFrame = Renderer::GetCurrentFrame();

				VkCommandBufferBeginInfo beginInfo{};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = 0;
				beginInfo.pInheritanceInfo = nullptr;

				VK_CHECK_RESULT(vkBeginCommandBuffer(mCommandBuffers[currentFrame], &beginInfo));
			});
	}
	
	void VulkanCommandBuffer::EndCommand() const
	{
		Renderer::Submit([this]()
			{
				uint32_t currentFrame = Renderer::GetCurrentFrame();

				VK_CHECK_RESULT(vkEndCommandBuffer(mCommandBuffers[currentFrame]));
			});
	}
	
	void VulkanCommandBuffer::Submit() const
	{
		Renderer::Submit([this]()
			{
				uint32_t currentFrame = Renderer::GetCurrentFrame();
				auto device = VulkanContext::GetLogicalDevice();
				
				VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

				VkSubmitInfo submitInfo{};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.pWaitDstStageMask = waitStages;
				submitInfo.waitSemaphoreCount = 0;
				submitInfo.pWaitSemaphores = nullptr;
				submitInfo.signalSemaphoreCount = 0;
				submitInfo.pSignalSemaphores = nullptr;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &mCommandBuffers[currentFrame];

				VK_CHECK_RESULT(vkResetFences(device, 1, &mFence));
				VK_CHECK_RESULT(vkQueueSubmit(VulkanContext::GetGraphicsQueue(), 1, &submitInfo, mFence));
				
				VK_CHECK_RESULT(vkWaitForFences(device, 1, &mFence, VK_TRUE, UINT64_MAX));
			});
	}
}