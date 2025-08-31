#include "ilargipch.h"

#define VMA_IMPLEMENTATION
#include "VulkanAllocator.h"
#include "VulkanContext.h"

namespace Ilargi
{
	VmaAllocator VulkanAllocator::sAllocator{ VK_NULL_HANDLE };

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
		
		aBuffer.allocation->SetName(sAllocator, "Buffer");
	}
	
	void VulkanAllocator::DestroyBuffer(VulkanBuffer& aBuffer)
	{
		vmaDestroyBuffer(sAllocator, aBuffer.buffer, aBuffer.allocation);
	}

	void VulkanAllocator::AllocateImage(Image& aImage, const VkImageCreateInfo& aImageInfo, VmaMemoryUsage aUsage)
	{
		VmaAllocationCreateInfo vmaAllocInfo = {};
		vmaAllocInfo.usage = aUsage;
		vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		vmaAllocInfo.priority = 1.0f;

		vmaCreateImage(sAllocator, &aImageInfo, &vmaAllocInfo, &aImage.image, &aImage.allocation, nullptr);
		aImage.allocation->SetName(sAllocator, "Image");
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