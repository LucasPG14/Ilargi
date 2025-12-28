#pragma once

#include <vma/vk_mem_alloc.h>

namespace Ilargi
{
	struct VulkanBuffer
	{
		VkBuffer buffer;
		VmaAllocation allocation;
	};

	struct Image
	{
		VkImage image;
		VmaAllocation allocation;
	};

	class VulkanAllocator
	{
	public:
		static void Init();
		static void Destroy();

		static void AllocateBuffer(VulkanBuffer& aBuffer, const VkBufferCreateInfo& aBufferInfo, VmaMemoryUsage aUsage, VmaAllocationInfo* aAllocationInfo = nullptr);
		static void DestroyBuffer(VulkanBuffer& aBuffer);

		static void AllocateImage(Image& aImage, const VkImageCreateInfo& aImageInfo, VmaMemoryUsage aUsage, const std::string& aDebugName = "Image");
		static void DestroyImage(Image& aImage);

		static void* MapMemory(const VulkanBuffer& aBuffer);
		static void UnmapMemory(VulkanBuffer& aBuffer);

		static void FlushAllocation(VmaAllocation aAllocation, uint32_t aSize);
	private:
		static VmaAllocator sAllocator;
	};
}