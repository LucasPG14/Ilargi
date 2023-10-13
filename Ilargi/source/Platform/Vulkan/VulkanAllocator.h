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

		static void AllocateBuffer(VulkanBuffer& buffer, const VkBufferCreateInfo& bufferInfo, VmaMemoryUsage usage);
		static void DestroyBuffer(VulkanBuffer& buffer);

		static void AllocateImage(Image& image, const VkImageCreateInfo& imageInfo, VmaMemoryUsage usage);
		static void DestroyImage(Image& image);

		static void* MapMemory(const VulkanBuffer& buffer);
		static void UnmapMemory(VulkanBuffer& buffer);

	private:
		static VmaAllocator allocator;
	};
}