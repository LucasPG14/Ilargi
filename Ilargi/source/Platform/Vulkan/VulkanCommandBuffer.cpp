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

		VkCommandBufferAllocateInfo allocInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, // sType
			nullptr,										// pNext
			VulkanContext::GetCommandPool(),				// commandPool
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,				// level
			static_cast<uint32_t>(mCommandBuffers.size())	// commandBufferCount
		};

		VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &allocInfo, mCommandBuffers.data()));

		VkFenceCreateInfo fenceInfo
		{
			VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,	// sType
			nullptr,								// pNext
			VK_FENCE_CREATE_SIGNALED_BIT			// flags
		};

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

				VkCommandBufferBeginInfo beginInfo
				{
					VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,	// sType
					nullptr,										// pNext		
					0,												// flags
					nullptr											// pInheritanceInfo
				};

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
				
				VkPipelineStageFlags waitStages[] { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

				VkSubmitInfo submitInfo
				{
					VK_STRUCTURE_TYPE_SUBMIT_INFO,	// sType
					nullptr,						// pNext								
					0,								// waitSemaphoreCount
					nullptr,						// pWaitSemaphores
					waitStages,						// pWaitDstStageMask
					1,								// commandBufferCount
					&mCommandBuffers[currentFrame], // pCommandBuffers
					0,								// signalSemaphoreCount
					nullptr							// pSignalSemaphores
				};

				VK_CHECK_RESULT(vkResetFences(device, 1, &mFence));
				VK_CHECK_RESULT(vkQueueSubmit(VulkanContext::GetGraphicsQueue(), 1, &submitInfo, mFence));
				
				VK_CHECK_RESULT(vkWaitForFences(device, 1, &mFence, VK_TRUE, UINT64_MAX));
			});
	}
}