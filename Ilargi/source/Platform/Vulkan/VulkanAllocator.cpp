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
	
	Buffer VulkanAllocator::AllocateBuffer(Buffer& buffer, const VkBufferCreateInfo& bufferInfo, VmaMemoryUsage usage)
	{
		VmaAllocationCreateInfo vmaAllocInfo = {};
		vmaAllocInfo.usage = usage;

		// Allocate the buffer
		vmaCreateBuffer(allocator, &bufferInfo, &vmaAllocInfo, &buffer.buffer, &buffer.allocation, nullptr);

		return buffer;
	}
	
	void VulkanAllocator::DestroyBuffer(Buffer& buffer)
	{
		vmaDestroyBuffer(allocator, buffer.buffer, buffer.allocation);
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