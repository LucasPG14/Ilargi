#include "ilargipch.h"

#include "VulkanTexture.h"
#include "Renderer/Renderer.h"
#include "VulkanContext.h"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Ilargi
{
	namespace Utils
	{
		VkFormat GetFormatFromChannels(int channels)
		{
			switch (channels)
			{
			case 1: return VK_FORMAT_R8_UNORM;
			case 2: return VK_FORMAT_R8G8_UNORM;
			case 3: return VK_FORMAT_R8G8B8A8_UNORM;
			case 4: return VK_FORMAT_R8G8B8A8_UNORM;
			}

			ILG_ASSERT(nullptr, "VkFormat not found for desired channels");
			return VkFormat();
		}
	}

	VulkanTexture2D::VulkanTexture2D(std::filesystem::path aFilepath) : mWidth(0), mHeight(0), mImage(), 
		mImageView(VK_NULL_HANDLE), mSampler(VK_NULL_HANDLE), mDescriptorSet(VK_NULL_HANDLE)
	{
		auto device{ VulkanContext::GetLogicalDevice() };

		int w, h, channels;

		stbi_set_flip_vertically_on_load(false);

		void* data{ stbi_load(aFilepath.string().c_str(), &w, &h, &channels, 4) };

		if (!data)
		{
			ILG_CORE_ERROR("Unable to load the texture: {0}", aFilepath.string());
			return;
		}

		mWidth = w;
		mHeight = h;

		VulkanBuffer buffer;

		VkDeviceSize imageSize{ mWidth * mHeight * 4 };

		VkBufferCreateInfo bufferInfo
		{
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,	// sType
			nullptr,								// pNext
			0,										// flags
			imageSize,								// size
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,		// usage
			VK_SHARING_MODE_EXCLUSIVE,				// sharingMode
			0,										// queueFamilyIndexCount
			nullptr									// pQueueFamilyIndices
		};

		VulkanAllocator::AllocateBuffer(buffer, bufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU);
		void* vkData{ VulkanAllocator::MapMemory(buffer) };

		memcpy(vkData, data, imageSize);

		VulkanAllocator::UnmapMemory(buffer);

		stbi_image_free(data);

		uint32_t mipLevels{ static_cast<uint32_t>(std::floor(std::log2(std::max(mWidth, mHeight)))) + 1 };

		VkImageCreateInfo imageInfo
		{
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,																// sType
			nullptr,																							// pNext
			0,																									// flags
			VK_IMAGE_TYPE_2D,																					// imageType
			VK_FORMAT_R8G8B8A8_SRGB,																			// format
			{																									// extent
				mWidth,																								// width
				mHeight,																							// height
				1																									// depth
			},
			mipLevels,																							// mipLevels
			1,																									// arrayLayers
			VK_SAMPLE_COUNT_1_BIT,																				// samples
			VK_IMAGE_TILING_OPTIMAL,																			// tiling
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,		// usage
			VK_SHARING_MODE_EXCLUSIVE,																			// sharingMode
			0,																									// queueFamilyIndexCount
			nullptr,																							// pQueueFamilyIndices
			VK_IMAGE_LAYOUT_UNDEFINED																			// initialLayout
		};

		VulkanAllocator::AllocateImage(mImage, imageInfo, VMA_MEMORY_USAGE_GPU_ONLY, "Texture2D");

		TransitionLayout(mipLevels, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		{
			VkCommandBuffer commandBuffer{ VulkanContext::BeginSingleCommandBuffer() };

			VkBufferImageCopy region
			{
				0,								// bufferOffset
				0,								// bufferRowLength
				0,								// bufferImageHeight
				{								// imageSubresource
					VK_IMAGE_ASPECT_COLOR_BIT,		// aspectMask
					0,								// mipLevel
					0,								// baseArrayLayer
					1,								// layerCount
				},
				{								// imageOffset
					0,								// x
					0,								// y
					0								// z
				}, 
				{								// imageExtent
					mWidth,							// width
					mHeight,						// height
					1								// depth
				} 
			};

			vkCmdCopyBufferToImage(commandBuffer, buffer.buffer, mImage.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			VulkanContext::EndSingleCommandBuffer(commandBuffer);
		}

		//TransitionLayout(mipLevels, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		VulkanAllocator::DestroyBuffer(buffer);
		
		// TODO: Check if the texture format is allowed to have MIPMAP_MODE_LINEAR
		GenerateMipMaps(mipLevels);

		VkImageViewCreateInfo viewInfo
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,	// sType
			nullptr,									// pNext
			0,											// flags
			mImage.image,								// image
			VK_IMAGE_VIEW_TYPE_2D,						// viewType
			VK_FORMAT_R8G8B8A8_SRGB,					// format
			{											// components

			},
			{											// subresourceRange
				VK_IMAGE_ASPECT_COLOR_BIT,					// aspectMask
				0,											// baseMipLevel
				mipLevels,									// levelCount
				0,											// baseArrayLayer
				1											// layerCount
			}
		};

		VK_CHECK_RESULT(vkCreateImageView(device, &viewInfo, nullptr, &mImageView));

		{
			VkSamplerCreateInfo samplerInfo
			{
				VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,	// sType
				nullptr,								// pNext
				0,										// flags
				VK_FILTER_LINEAR,						// magFilter
				VK_FILTER_LINEAR,						// minFilter
				VK_SAMPLER_MIPMAP_MODE_LINEAR,			// mipmapMode
				VK_SAMPLER_ADDRESS_MODE_REPEAT,			// addressModeU
				VK_SAMPLER_ADDRESS_MODE_REPEAT,			// addressModeV
				VK_SAMPLER_ADDRESS_MODE_REPEAT,			// addressModeW
				0.0f,									// mipLodBias
				VK_FALSE,								// anisotropyEnable
				1.0f,									// maxAnisotropy
				VK_FALSE,								// compareEnable
				VK_COMPARE_OP_ALWAYS,					// compareOp
				0.0f,									// minLod
				static_cast<float>(mipLevels),			// maxLod
				VK_BORDER_COLOR_INT_OPAQUE_BLACK,		// borderColor
				VK_FALSE								// unnormalizedCoordinates
			};

			VK_CHECK_RESULT(vkCreateSampler(device, &samplerInfo, nullptr, &mSampler));
		}

		mDescriptorSet = ImGui_ImplVulkan_AddTexture(mSampler, mImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	VulkanTexture2D::VulkanTexture2D(void* aData, int aWidth, int aHeight, int aChannels) : mWidth(aWidth), mHeight(aHeight), mImage(),
		mImageView(VK_NULL_HANDLE), mSampler(VK_NULL_HANDLE), mDescriptorSet(VK_NULL_HANDLE)
	{
		auto device{ VulkanContext::GetLogicalDevice() };

		VulkanBuffer buffer;

		VkFormat format{ Utils::GetFormatFromChannels(aChannels) };

		VkDeviceSize imageSize{ mWidth * mHeight * aChannels };

		VkBufferCreateInfo bufferInfo
		{
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,	// sType
			nullptr,								// pNext
			0,										// flags
			imageSize,								// size
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,		// usage
			VK_SHARING_MODE_EXCLUSIVE,				// sharingMode
			0,										// queueFamilyIndexCount
			nullptr									// pQueueFamilyIndices
		};

		VulkanAllocator::AllocateBuffer(buffer, bufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU);
		void* vkData{ VulkanAllocator::MapMemory(buffer) };

		memcpy(vkData, aData, imageSize);

		VulkanAllocator::UnmapMemory(buffer);

		uint32_t mipLevels{ static_cast<uint32_t>(std::floor(std::log2(std::max(mWidth, mHeight)))) + 1 };

		VkImageCreateInfo imageInfo
		{
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,																// sType
			nullptr,																							// pNext
			0,																									// flags
			VK_IMAGE_TYPE_2D,																					// imageType
			format,																								// format
			{																									// extent
				mWidth,																								// width
				mHeight,																							// height
				1																									// depth
			},
			mipLevels,																							// mipLevels
			1,																									// arrayLayers
			VK_SAMPLE_COUNT_1_BIT,																				// samples
			VK_IMAGE_TILING_OPTIMAL,																			// tiling
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,		// usage
			VK_SHARING_MODE_EXCLUSIVE,																			// sharingMode
			0,																									// queueFamilyIndexCount
			nullptr,																							// pQueueFamilyIndices
			VK_IMAGE_LAYOUT_UNDEFINED																			// initialLayout
		};

		VulkanAllocator::AllocateImage(mImage, imageInfo, VMA_MEMORY_USAGE_GPU_ONLY, "Texture2D");

		TransitionLayout(mipLevels, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		{
			VkCommandBuffer commandBuffer{ VulkanContext::BeginSingleCommandBuffer() };

			VkBufferImageCopy region
			{
				0,								// bufferOffset
				0,								// bufferRowLength
				0,								// bufferImageHeight
				{								// imageSubresource
					VK_IMAGE_ASPECT_COLOR_BIT,		// aspectMask
					0,								// mipLevel
					0,								// baseArrayLayer
					1,								// layerCount
				},
				{								// imageOffset
					0,								// x
					0,								// y
					0								// z
				},
				{								// imageExtent
					mWidth,							// width
					mHeight,						// height
					1								// depth
				}
			};

			vkCmdCopyBufferToImage(commandBuffer, buffer.buffer, mImage.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			VulkanContext::EndSingleCommandBuffer(commandBuffer);
		}

		//TransitionLayout(mipLevels, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		VulkanAllocator::DestroyBuffer(buffer);

		// TODO: Check if the texture format is allowed to have MIPMAP_MODE_LINEAR
		GenerateMipMaps(mipLevels);

		VkImageViewCreateInfo viewInfo
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,	// sType
			nullptr,									// pNext
			0,											// flags
			mImage.image,								// image
			VK_IMAGE_VIEW_TYPE_2D,						// viewType
			format,										// format
			{											// components

			},
			{											// subresourceRange
				VK_IMAGE_ASPECT_COLOR_BIT,					// aspectMask
				0,											// baseMipLevel
				mipLevels,									// levelCount
				0,											// baseArrayLayer
				1											// layerCount
			}
		};

		VK_CHECK_RESULT(vkCreateImageView(device, &viewInfo, nullptr, &mImageView));

		// Creating sampler
		{
			VkSamplerCreateInfo samplerInfo
			{
				VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,	// sType
				nullptr,								// pNext
				0,										// flags
				VK_FILTER_LINEAR,						// magFilter
				VK_FILTER_LINEAR,						// minFilter
				VK_SAMPLER_MIPMAP_MODE_LINEAR,			// mipmapMode
				VK_SAMPLER_ADDRESS_MODE_REPEAT,			// addressModeU
				VK_SAMPLER_ADDRESS_MODE_REPEAT,			// addressModeV
				VK_SAMPLER_ADDRESS_MODE_REPEAT,			// addressModeW
				0.0f,									// mipLodBias
				VK_FALSE,								// anisotropyEnable
				1.0f,									// maxAnisotropy
				VK_FALSE,								// compareEnable
				VK_COMPARE_OP_ALWAYS,					// compareOp
				0.0f,									// minLod
				static_cast<float>(mipLevels),			// maxLod
				VK_BORDER_COLOR_INT_OPAQUE_BLACK,		// borderColor
				VK_FALSE								// unnormalizedCoordinates
			};

			VK_CHECK_RESULT(vkCreateSampler(device, &samplerInfo, nullptr, &mSampler));
		}

		mDescriptorSet = ImGui_ImplVulkan_AddTexture(mSampler, mImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
	
	VulkanTexture2D::~VulkanTexture2D()
	{
		auto device{ VulkanContext::GetLogicalDevice() };

		ImGui_ImplVulkan_RemoveTexture(mDescriptorSet);

		VulkanAllocator::DestroyImage(mImage);
		vkDestroySampler(device, mSampler, nullptr);
		vkDestroyImageView(device, mImageView, nullptr);
	}
	
	void VulkanTexture2D::TransitionLayout(uint32_t aMipLevels, VkImageLayout aOldLayout, VkImageLayout aNewLayout)
	{
		// Transitioning image
		VkCommandBuffer commandBuffer{ VulkanContext::BeginSingleCommandBuffer() };

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = aOldLayout;
		barrier.newLayout = aNewLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = mImage.image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = aMipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage{ 0 };
		VkPipelineStageFlags destinationStage{ 0 };

		if (aOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && aNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (aOldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && aNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		VulkanContext::EndSingleCommandBuffer(commandBuffer);
	}
	
	void VulkanTexture2D::GenerateMipMaps(uint32_t aMipLevels)
	{
		VkCommandBuffer commandBuffer{ VulkanContext::BeginSingleCommandBuffer() };

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = mImage.image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth{ static_cast<int32_t>(mWidth) };
		int32_t mipHeight{ static_cast<int32_t>(mHeight) };

		for (uint32_t i { 1 }; i < aMipLevels; ++i)
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 
				nullptr, 0, nullptr, 1, &barrier);
		
			VkImageBlit blit {};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };

			mipWidth = mipWidth > 1 ? static_cast<int32_t>(mipWidth * 0.5f) : 1;
			mipHeight = mipHeight > 1 ? static_cast<int32_t>(mipHeight * 0.5f) : 1;
			blit.dstOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(commandBuffer, mImage.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, mImage.image, 
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
		
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 
				nullptr, 0, nullptr, 1, &barrier);
		}

		barrier.subresourceRange.baseMipLevel = aMipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 
			nullptr, 0, nullptr, 1, &barrier);

		VulkanContext::EndSingleCommandBuffer(commandBuffer);
	}
}