#include "ilargipch.h"

#include "VulkanVertexBuffer.h"
#include "Renderer/Renderer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanContext.h"

namespace Ilargi
{
	VulkanVertexBuffer::VulkanVertexBuffer(void* aData, uint32_t aSize) 
	{
		VkBufferCreateInfo vertexBufferInfo
		{
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,	// sType
			nullptr,								// pNext
			0,										// flags
			aSize,									// size
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,		// usage
			VK_SHARING_MODE_EXCLUSIVE,				// sharingMode
			0,										// queueFamilyIndexCount
			nullptr									// pQueueFamilyIndices
		};

		// TODO: Should be only on the GPU using a staging buffer
		VulkanAllocator::AllocateBuffer(mBuffer, vertexBufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU);
		void* bufferData{ VulkanAllocator::MapMemory(mBuffer) };
		memcpy(bufferData, aData, aSize);
		VulkanAllocator::UnmapMemory(mBuffer);
	}
	
	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		vkDeviceWaitIdle(VulkanContext::GetLogicalDevice());

		VulkanAllocator::DestroyBuffer(mBuffer);
	}

	void VulkanVertexBuffer::Bind(std::shared_ptr<CommandBuffer> aCommandBuffer) const
	{
		uint32_t currentFrame{ Renderer::GetCurrentFrame() };
		auto cmdBuffer{ aCommandBuffer->As<VulkanCommandBuffer>() };

		VkDeviceSize offset{ 0 };
		vkCmdBindVertexBuffers(cmdBuffer->GetCurrentCommand(currentFrame), 0, 1, &mBuffer.buffer, &offset);
	}

	void VulkanVertexBuffer::Destroy()
	{
		vkDeviceWaitIdle(VulkanContext::GetLogicalDevice());

		VulkanAllocator::DestroyBuffer(mBuffer);
	}
}