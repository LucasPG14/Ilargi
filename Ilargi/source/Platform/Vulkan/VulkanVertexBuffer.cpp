#include "ilargipch.h"

#include "VulkanVertexBuffer.h"
#include "Renderer/Renderer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanGraphicsContext.h"

namespace Ilargi
{
	VulkanVertexBuffer::VulkanVertexBuffer(void* aData, uint32_t aSize) 
	{
		VkBufferCreateInfo vertexBufferInfo
		{
			.sType {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO},
			.pNext {nullptr},
			.flags {0U},
			.size {aSize},
			.usage {VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT},
			.sharingMode {VK_SHARING_MODE_EXCLUSIVE},
			.queueFamilyIndexCount {0U},
			.pQueueFamilyIndices {nullptr}
		};

		VulkanAllocator::AllocateBuffer(mBuffer, vertexBufferInfo, VMA_MEMORY_USAGE_GPU_ONLY);
		{
			VkBufferCreateInfo stagingBufferInfo
			{
				.sType {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO},
				.pNext {nullptr},
				.flags {0U},
				.size {aSize},
				.usage {VK_BUFFER_USAGE_TRANSFER_SRC_BIT},
				.sharingMode {VK_SHARING_MODE_EXCLUSIVE},
				.queueFamilyIndexCount {0U},
				.pQueueFamilyIndices {nullptr}
			};

			VulkanBuffer stagingBuffer;
			VmaAllocationInfo allocationInfo {};
			VulkanAllocator::AllocateBuffer(stagingBuffer, stagingBufferInfo, VMA_MEMORY_USAGE_CPU_ONLY, &allocationInfo);

			ILG_ASSERT(!allocationInfo.pMappedData, "AllocationInfo must have value");

			void* data{ VulkanAllocator::MapMemory(stagingBuffer) };
			memcpy(data, aData, aSize);
			VulkanAllocator::UnmapMemory(stagingBuffer);

			auto commandBuffer{ VulkanGraphicsContext::BeginSingleCommandBuffer() };

			VkBufferCopy copy
			{
				.srcOffset {0U},
				.dstOffset {0U},
				.size {aSize}
			};

			vkCmdCopyBuffer(commandBuffer, stagingBuffer.buffer, mBuffer.buffer, 1, &copy);

			VulkanGraphicsContext::EndSingleCommandBuffer(commandBuffer);

			VulkanAllocator::DestroyBuffer(stagingBuffer);
		}
	}
	
	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		vkDeviceWaitIdle(VulkanGraphicsContext::GetLogicalDevice());

		VulkanAllocator::DestroyBuffer(mBuffer);
	}

	void VulkanVertexBuffer::Bind(const std::shared_ptr<ICommandBuffer>& aCommandBuffer) const
	{
		uint32_t currentFrame{ Renderer::GetCurrentFrame() };
		auto cmdBuffer{ aCommandBuffer->As<VulkanCommandBuffer>() };

		VkDeviceSize offset{ 0 };
		vkCmdBindVertexBuffers(cmdBuffer->GetCurrentCommand(currentFrame), 0, 1, &mBuffer.buffer, &offset);
	}

	void VulkanVertexBuffer::Destroy()
	{
		vkDeviceWaitIdle(VulkanGraphicsContext::GetLogicalDevice());

		VulkanAllocator::DestroyBuffer(mBuffer);
	}
}