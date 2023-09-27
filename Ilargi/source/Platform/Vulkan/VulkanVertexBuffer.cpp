#include "ilargipch.h"

#include "VulkanVertexBuffer.h"
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

	void VulkanVertexBuffer::Destroy()
	{
		VulkanAllocator::DestroyBuffer(buffer);
	}
	
	void VulkanVertexBuffer::Bind(VkCommandBuffer cmdBuffer)
	{
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &buffer.buffer, &offset);
	}
}