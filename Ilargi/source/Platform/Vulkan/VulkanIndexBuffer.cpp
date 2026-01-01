#include "ilargipch.h"

#include "VulkanIndexBuffer.h"
#include "Renderer/Renderer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanContext.h"

namespace Ilargi
{
	VulkanIndexBuffer::VulkanIndexBuffer(void* aData, uint32_t aIndicesCount) : mCount(aIndicesCount)
	{
		VkBufferCreateInfo indexBufferInfo
		{
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,									// sType
			nullptr,																// pNext
			0,																		// flags
			mCount * sizeof(uint32_t),												// size
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,	// usage
			VK_SHARING_MODE_EXCLUSIVE,												// sharingMode
			0,																		// queueFamilyIndexCount
			nullptr																	// pQueueFamilyIndices
		};

		VulkanAllocator::AllocateBuffer(mBuffer, indexBufferInfo, VMA_MEMORY_USAGE_GPU_ONLY);
		{
			VkBufferCreateInfo stagingBufferInfo
			{
				VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				nullptr,
				0,
				mCount * sizeof(uint32_t),
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
			memcpy(data, aData, mCount * sizeof(uint32_t));
			VulkanAllocator::UnmapMemory(stagingBuffer);

			auto commandBuffer{ VulkanContext::BeginSingleCommandBuffer() };

			VkBufferCopy copy
			{
				0,							// srcOffset
				0,							// dstOffset
				mCount * sizeof(uint32_t)	// size
			};

			vkCmdCopyBuffer(commandBuffer, stagingBuffer.buffer, mBuffer.buffer, 1, &copy);

			VulkanContext::EndSingleCommandBuffer(commandBuffer);

			VulkanAllocator::DestroyBuffer(stagingBuffer);
		}
	}
	
	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		VulkanAllocator::DestroyBuffer(mBuffer);
	}

	void VulkanIndexBuffer::Bind(const std::shared_ptr<CommandBuffer>& commandBuffer) const
	{
		uint32_t currentFrame{ Renderer::GetCurrentFrame() };
		auto cmdBuffer{ commandBuffer->As<VulkanCommandBuffer>() };
		
		vkCmdBindIndexBuffer(cmdBuffer->GetCurrentCommand(currentFrame), mBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void VulkanIndexBuffer::Destroy()
	{
		VulkanAllocator::DestroyBuffer(mBuffer);
	}
}