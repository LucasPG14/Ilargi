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
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,										// sType
			nullptr,																	// pNext
			0,																			// flags
			aSize,																		// size
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,		// usage
			VK_SHARING_MODE_EXCLUSIVE,													// sharingMode
			0,																			// queueFamilyIndexCount
			nullptr																		// pQueueFamilyIndices
		};

		VulkanAllocator::AllocateBuffer(mBuffer, vertexBufferInfo, VMA_MEMORY_USAGE_GPU_ONLY);
		{
			VkBufferCreateInfo stagingBufferInfo
			{
				VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				nullptr,
				0,
				aSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_SHARING_MODE_EXCLUSIVE,
				0,
				nullptr
			};

			VulkanBuffer stagingBuffer;
			VmaAllocationInfo allocationInfo {};
			VulkanAllocator::AllocateBuffer(stagingBuffer, stagingBufferInfo, VMA_MEMORY_USAGE_CPU_ONLY, &allocationInfo);

			ILG_ASSERT(!allocationInfo.pMappedData, "AllocationInfo must have value");

			void* data{ VulkanAllocator::MapMemory(stagingBuffer) };
			memcpy(data, aData, aSize);
			VulkanAllocator::UnmapMemory(stagingBuffer);

			auto commandBuffer{ VulkanContext::BeginSingleCommandBuffer() };

			VkBufferCopy copy
			{
				0,		// srcOffset
				0,		// dstOffset
				aSize	// size
			};

			vkCmdCopyBuffer(commandBuffer, stagingBuffer.buffer, mBuffer.buffer, 1, &copy);

			VulkanContext::EndSingleCommandBuffer(commandBuffer);

			VulkanAllocator::DestroyBuffer(stagingBuffer);
		}
	}
	
	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		vkDeviceWaitIdle(VulkanContext::GetLogicalDevice());

		VulkanAllocator::DestroyBuffer(mBuffer);
	}

	void VulkanVertexBuffer::Bind(const std::shared_ptr<CommandBuffer>& aCommandBuffer) const
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