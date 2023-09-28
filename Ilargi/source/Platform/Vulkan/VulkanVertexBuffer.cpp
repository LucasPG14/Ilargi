#include "ilargipch.h"

#include "VulkanVertexBuffer.h"
#include "Renderer/Renderer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanContext.h"

namespace Ilargi
{
	VulkanVertexBuffer::VulkanVertexBuffer(void* data, uint32_t size) 
	{
		VkBufferCreateInfo vertexBufferInfo = {};
		vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertexBufferInfo.size = size;
		vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		// TODO: Should be only on the GPU using a staging buffer
		VulkanAllocator::AllocateBuffer(buffer, vertexBufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU);
		void* bufferData = VulkanAllocator::MapMemory(buffer);
		memcpy(bufferData, data, size);
		VulkanAllocator::UnmapMemory(buffer);
	}
	
	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
	}

	void VulkanVertexBuffer::Bind(std::shared_ptr<CommandBuffer> commandBuffer) const
	{
		uint32_t currentFrame = Renderer::GetCurrentFrame();
		auto cmdBuffer = std::static_pointer_cast<VulkanCommandBuffer>(commandBuffer);

		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(cmdBuffer->GetCurrentCommand(currentFrame), 0, 1, &buffer.buffer, &offset);
	}

	void VulkanVertexBuffer::Destroy()
	{
		vkDeviceWaitIdle(VulkanContext::GetLogicalDevice());

		VulkanAllocator::DestroyBuffer(buffer);
	}
	
	void VulkanVertexBuffer::Bind(VkCommandBuffer cmdBuffer)
	{
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &buffer.buffer, &offset);
	}
}