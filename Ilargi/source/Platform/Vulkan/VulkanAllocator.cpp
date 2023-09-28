#include "ilargipch.h"

#define VMA_IMPLEMENTATION
#include "VulkanAllocator.h"
#include "VulkanContext.h"

namespace Ilargi
{
	VmaAllocator VulkanAllocator::allocator = VK_NULL_HANDLE;

	void VulkanAllocator::Init()
	{
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = VulkanContext::GetPhysicalDevice();
		allocatorInfo.device = VulkanContext::GetLogicalDevice();
		allocatorInfo.instance = VulkanContext::GetInstance();

		vmaCreateAllocator(&allocatorInfo, &allocator);
	}
	
	void VulkanAllocator::Destroy()
	{
		vmaDestroyAllocator(allocator);
	}
	
	void VulkanAllocator::AllocateBuffer(Buffer& buffer, const VkBufferCreateInfo& bufferInfo, VmaMemoryUsage usage)
	{
		VmaAllocationCreateInfo vmaAllocInfo = {};
		vmaAllocInfo.usage = usage;

		// Allocate the buffer
		vmaCreateBuffer(allocator, &bufferInfo, &vmaAllocInfo, &buffer.buffer, &buffer.allocation, nullptr);
	}
	
	void VulkanAllocator::DestroyBuffer(Buffer& buffer)
	{
		vmaDestroyBuffer(allocator, buffer.buffer, buffer.allocation);
	}

	void VulkanAllocator::AllocateImage(Image& image, const VkImageCreateInfo& imageInfo, VmaMemoryUsage usage)
	{
		VmaAllocationCreateInfo vmaallocInfo = {};
		vmaallocInfo.usage = usage;
		vmaallocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		vmaallocInfo.priority = 1.0f;

		vmaCreateImage(allocator, &imageInfo, &vmaallocInfo, &image.image, &image.allocation, nullptr);
	}

	void VulkanAllocator::DestroyImage(Image& image)
	{
		vmaDestroyImage(allocator, image.image, image.allocation);
	}
	
	void* VulkanAllocator::MapMemory(const Buffer& buffer)
	{
		void* data;
		vmaMapMemory(allocator, buffer.allocation, &data);

		return data;
	}
	
	void VulkanAllocator::UnmapMemory(Buffer& buffer)
	{
		vmaUnmapMemory(allocator, buffer.allocation);
	}
}