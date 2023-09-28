#pragma once

#include <vma/vk_mem_alloc.h>

namespace Ilargi
{
	struct Buffer
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

		static void AllocateBuffer(Buffer& buffer, const VkBufferCreateInfo& bufferInfo, VmaMemoryUsage usage);
		static void DestroyBuffer(Buffer& buffer);

		static void AllocateImage(Image& image, const VkImageCreateInfo& imageInfo, VmaMemoryUsage usage);
		static void DestroyImage(Image& image);

		static void* MapMemory(const Buffer& buffer);
		static void UnmapMemory(Buffer& buffer);

	private:
		static VmaAllocator allocator;
	};
}