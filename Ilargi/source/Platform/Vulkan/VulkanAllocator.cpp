#include "ilargipch.h"

#define VMA_IMPLEMENTATION
#include "VulkanAllocator.h"
#include "VulkanContext.h"

namespace Ilargi
{
	VmaAllocator VulkanAllocator::sAllocator = VK_NULL_HANDLE;

	void VulkanAllocator::Init()
	{
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = VulkanContext::GetPhysicalDevice();
		allocatorInfo.device = VulkanContext::GetLogicalDevice();
		allocatorInfo.instance = VulkanContext::GetInstance();

		vmaCreateAllocator(&allocatorInfo, &sAllocator);
	}
	
	void VulkanAllocator::Destroy()
	{
		vmaDestroyAllocator(sAllocator);
	}
	
	void VulkanAllocator::AllocateBuffer(VulkanBuffer& aBuffer, const VkBufferCreateInfo& aBufferInfo, VmaMemoryUsage aUsage)
	{
		VmaAllocationCreateInfo vmaAllocInfo = {};
		vmaAllocInfo.usage = aUsage;

		// Allocate the buffer
		vmaCreateBuffer(sAllocator, &aBufferInfo, &vmaAllocInfo, &aBuffer.buffer, &aBuffer.allocation, nullptr);
	}
	
	void VulkanAllocator::DestroyBuffer(VulkanBuffer& aBuffer)
	{
		vmaDestroyBuffer(sAllocator, aBuffer.buffer, aBuffer.allocation);
	}

	void VulkanAllocator::AllocateImage(Image& aImage, const VkImageCreateInfo& aImageInfo, VmaMemoryUsage aUsage)
	{
		VmaAllocationCreateInfo vmaallocInfo = {};
		vmaallocInfo.usage = aUsage;
		vmaallocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		vmaallocInfo.priority = 1.0f;

		vmaCreateImage(sAllocator, &aImageInfo, &vmaallocInfo, &aImage.image, &aImage.allocation, nullptr);
	}

	void VulkanAllocator::DestroyImage(Image& aImage)
	{
		vmaDestroyImage(sAllocator, aImage.image, aImage.allocation);
	}
	
	void* VulkanAllocator::MapMemory(const VulkanBuffer& aBuffer)
	{
		void* data;
		vmaMapMemory(sAllocator, aBuffer.allocation, &data);

		return data;
	}
	
	void VulkanAllocator::UnmapMemory(VulkanBuffer& aBuffer)
	{
		vmaUnmapMemory(sAllocator, aBuffer.allocation);
	}
}