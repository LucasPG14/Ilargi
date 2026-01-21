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
		/*
		* @brief Initializes the Vulkan allocator
		*/
		static void Init();

		/*
		* @brief Destroys the Vulkan allocator
		*/
		static void Destroy();

		/*
		* @brief Allocates a buffer.
		* @param aBuffer The buffer to allocate.
		* @param aBufferInfo The information of the buffer to allocate.
		* @param aUsage Type of memory for the buffer.
		* @param aAllocationInfo The information of the allocation.
		*/
		static void AllocateBuffer(VulkanBuffer& aBuffer, const VkBufferCreateInfo& aBufferInfo, VmaMemoryUsage aUsage, VmaAllocationInfo* aAllocationInfo = nullptr);
		
		/*
		* @brief Destroys an allocated buffer.
		* @param aBuffer The buffer to deallocate.
		*/
		static void DestroyBuffer(VulkanBuffer& aBuffer);

		/*
		* @brief Allocates a image.
		* @param aImage The image to allocate.
		* @param aImageInfo The information of the image to allocate.
		* @param aUsage Type of memory for the image.
		* @param aDebugName The debug name for the image allocated.
		*/
		static void AllocateImage(Image& aImage, const VkImageCreateInfo& aImageInfo, VmaMemoryUsage aUsage, const std::string& aDebugName = "Image");
		
		/*
		* @brief Destroys an allocated image.
		* @param aImage The image to deallocate.
		*/
		static void DestroyImage(Image& aImage);

		/*
		* @brief Returns a pointer to the reserved memory for a buffer.
		* @param aBuffer The buffer to reserve the data.
		* @return Pointer to the reserved data.
		*/
		static void* MapMemory(const VulkanBuffer& aBuffer);

		/*
		* @brief Releases the reserved memory for a buffer.
		* @param aBuffer The buffer to release the data.
		*/
		static void UnmapMemory(VulkanBuffer& aBuffer);

	private:
		static VmaAllocator sAllocator; // Instance of the Vulkan allocator.
	};
}