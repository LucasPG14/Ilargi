#include "ilargipch.h"

#include "VulkanCommandBuffer.h"
#include "VulkanContext.h"
#include "Renderer/Renderer.h"

namespace Ilargi
{
	VulkanCommandBuffer::VulkanCommandBuffer(uint32_t framesInFlight)
	{
		auto device = VulkanContext::GetLogicalDevice();

		commandBuffers.resize(framesInFlight);

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = VulkanContext::GetCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) == VK_SUCCESS, "Unable to allocate the command buffers");
	}
	
	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
	}

	void VulkanCommandBuffer::BeginCommand() const
	{
		uint32_t currentFrame = Renderer::GetCurrentFrame();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo) == VK_SUCCESS, "Failed to begin command buffer");
	}
	
	void VulkanCommandBuffer::EndCommand() const
	{
		uint32_t currentFrame = Renderer::GetCurrentFrame();

		VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffers[currentFrame]) == VK_SUCCESS, "Failed to end command buffer");
	}
}