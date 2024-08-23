#include "ilargipch.h"

#include "VulkanIndexBuffer.h"
#include "Renderer/Renderer.h"
#include "VulkanCommandBuffer.h"

namespace Ilargi
{
	VulkanIndexBuffer::VulkanIndexBuffer(void* data, uint32_t indicesCount) : mCount(indicesCount)
	{
		VkBufferCreateInfo indexBufferInfo = {};
		indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		indexBufferInfo.size = mCount * sizeof(uint32_t);
		indexBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		indexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		// TODO: Should be only on the GPU using a staging buffer
		VulkanAllocator::AllocateBuffer(mBuffer, indexBufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU);
		void* bufferData = VulkanAllocator::MapMemory(mBuffer);
		memcpy(bufferData, data, indexBufferInfo.size);
		VulkanAllocator::UnmapMemory(mBuffer);
	}
	
	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		VulkanAllocator::DestroyBuffer(mBuffer);
	}

	void VulkanIndexBuffer::Bind(std::shared_ptr<CommandBuffer> commandBuffer) const
	{
		uint32_t currentFrame = Renderer::GetCurrentFrame();
		auto cmdBuffer = std::static_pointer_cast<VulkanCommandBuffer>(commandBuffer);
		
		vkCmdBindIndexBuffer(cmdBuffer->GetCurrentCommand(currentFrame), mBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void VulkanIndexBuffer::Destroy()
	{
		VulkanAllocator::DestroyBuffer(mBuffer);
	}
}