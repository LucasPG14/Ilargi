#pragma once

#include <vma/vk_mem_alloc.h>

namespace Ilargi
{
	struct Buffer
	{
		VkBuffer buffer;
		VmaAllocation allocation;
	};

	class VulkanAllocator
	{
	public:
		static void Init();
		static void Destroy();

		static Buffer AllocateBuffer(Buffer& buffer, const VkBufferCreateInfo& bufferInfo, VmaMemoryUsage usage);
		static void DestroyBuffer(Buffer& buffer);

		static void* MapMemory(const Buffer& buffer);
		static void UnmapMemory(Buffer& buffer);

	private:
		static VmaAllocator allocator;
	};
}