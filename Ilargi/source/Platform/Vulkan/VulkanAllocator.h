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

		static void AllocateBuffer(VulkanBuffer& aBuffer, const VkBufferCreateInfo& aBufferInfo, VmaMemoryUsage aUsage);
		static void DestroyBuffer(VulkanBuffer& aBuffer);

		static void AllocateImage(Image& aImage, const VkImageCreateInfo& aImageInfo, VmaMemoryUsage aUsage);
		static void DestroyImage(Image& aImage);

		static void* MapMemory(const VulkanBuffer& aBuffer);
		static void UnmapMemory(VulkanBuffer& aBuffer);

	private:
		static VmaAllocator sAllocator;
	};
}