#include "ilargipch.h"

// Main headers
#include "VulkanFramebuffer.h"
#include "Renderer/Renderer.h"
#include "VulkanGraphicsContext.h"
#include "VulkanRenderPass.h"

namespace Ilargi
{
	VulkanFramebuffer::VulkanFramebuffer(const FramebufferProperties& aProperties) 
		: mProperties(aProperties), mDepthSpecification(ImageFormat::NONE), mDepthAttachment(), mFramebuffer(VK_NULL_HANDLE), 
		mSampler(VK_NULL_HANDLE), mDescriptorSetLayout(VK_NULL_HANDLE), mDescriptorSet(VK_NULL_HANDLE)
	{
		for (ImageFormat format : aProperties.Formats)
		{
			if (Utils::IsDepth(format))
			{
				mDepthSpecification = format; 
				continue;
			}
			mColorSpecifications.push_back(format);
		}

		Init();
	}
	
	VulkanFramebuffer::~VulkanFramebuffer()
	{
	}

	void VulkanFramebuffer::Init()
	{
		auto device{ VulkanGraphicsContext::GetLogicalDevice() };
		
		std::vector<VkImageView> attachments;

		uint32_t maxSamples{ Renderer::GetConfig().maxAASamples };

		mColorAttachments.resize(mColorSpecifications.size());
		int i{ 0 };
		for (VulkanAttachment& attachment : mColorAttachments)
		{
			VkFormat format{ Utils::GetFormatFromImageFormat(mColorSpecifications[i++]) };

			VkImageCreateInfo imageInfo
			{
				.sType {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO},
				.pNext {nullptr},
				.flags {0U},
				.imageType {VK_IMAGE_TYPE_2D},
				.format {format},
				.extent {.width{mProperties.Width}, .height{mProperties.Height}, .depth{1U}},
				.mipLevels {1U},
				.arrayLayers {1U},
				.samples {VK_SAMPLE_COUNT_1_BIT},
				.tiling {VK_IMAGE_TILING_OPTIMAL},
				.usage {VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT},
				.sharingMode {VK_SHARING_MODE_EXCLUSIVE},
				.queueFamilyIndexCount {0U},
				.pQueueFamilyIndices {nullptr},
				.initialLayout {VK_IMAGE_LAYOUT_UNDEFINED}
			};

			VulkanAllocator::AllocateImage(attachment.image, imageInfo, VMA_MEMORY_USAGE_GPU_ONLY, "Framebuffer");

			VkImageViewCreateInfo imageViewInfo
			{
				.sType {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO},
				.pNext {nullptr},
				.flags {0U},
				.image {attachment.image.image},
				.viewType {VK_IMAGE_VIEW_TYPE_2D},
				.format {format},
				.components {VK_COMPONENT_SWIZZLE_IDENTITY},
				.subresourceRange 
				{
					.aspectMask {VK_IMAGE_ASPECT_COLOR_BIT},
					.baseMipLevel {0U},
					.levelCount {1U},
					.baseArrayLayer {0U},
					.layerCount {1U}
				}
			};

			VK_CHECK_RESULT(vkCreateImageView(device, &imageViewInfo, nullptr, &attachment.imageView));

			attachments.push_back(attachment.imageView);
		}

		// Creating the depth image
		if (mDepthSpecification != ImageFormat::NONE)
		{
			VkFormat depthFormat{ Utils::GetFormatFromImageFormat(mDepthSpecification) };

			VkImageCreateInfo imageInfo
			{
				.sType {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO},
				.pNext {nullptr},
				.flags {0U},
				.imageType {VK_IMAGE_TYPE_2D},
				.format {depthFormat},
				.extent {.width{mProperties.Width}, .height{mProperties.Height}, .depth{1U}},
				.mipLevels {1U},
				.arrayLayers {1U},
				.samples {VK_SAMPLE_COUNT_1_BIT},
				.tiling {VK_IMAGE_TILING_OPTIMAL},
				.usage {VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT},
				.sharingMode {VK_SHARING_MODE_EXCLUSIVE},
				.queueFamilyIndexCount {0U},
				.pQueueFamilyIndices {nullptr},
				.initialLayout {VK_IMAGE_LAYOUT_UNDEFINED}
			};

			VulkanAllocator::AllocateImage(mDepthAttachment.image, imageInfo, VMA_MEMORY_USAGE_GPU_ONLY, "Framebuffer");

			VkImageViewCreateInfo imageViewInfo
			{
				.sType {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO},
				.pNext {nullptr},
				.flags {0U},
				.image {mDepthAttachment.image.image},
				.viewType {VK_IMAGE_VIEW_TYPE_2D},
				.format {depthFormat},
				.components {VK_COMPONENT_SWIZZLE_IDENTITY},
				.subresourceRange
				{
					.aspectMask {VK_IMAGE_ASPECT_DEPTH_BIT},
					.baseMipLevel {0U},
					.levelCount {1U},
					.baseArrayLayer {0U},
					.layerCount {1U}
				}
			};
			
			VK_CHECK_RESULT(vkCreateImageView(device, &imageViewInfo, nullptr, &mDepthAttachment.imageView));

			attachments.push_back(mDepthAttachment.imageView);
		}

		const VkRenderPass& renderPass{ Renderer::GetRenderPass({mProperties.Formats}).As<VulkanRenderPass>().GetRenderPass() };
		// Creating the framebuffer
		{
			VkFramebufferCreateInfo framebufferInfo
			{
				.sType {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO},
				.pNext {nullptr},
				.flags {0U},
				.renderPass {renderPass},
				.attachmentCount {static_cast<uint32_t>(attachments.size())},
				.pAttachments {attachments.data()},
				.width {mProperties.Width},
				.height {mProperties.Height},
				.layers {1U}
			};

			VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &mFramebuffer));
		}

		// Sampler
		if (!mSampler) 
		{
			VkSamplerCreateInfo samplerInfo
			{
				.sType {VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO},
				.pNext {nullptr},
				.flags {0U},
				.magFilter {VK_FILTER_LINEAR},
				.minFilter {VK_FILTER_LINEAR},
				.mipmapMode {VK_SAMPLER_MIPMAP_MODE_LINEAR},
				.addressModeU {VK_SAMPLER_ADDRESS_MODE_REPEAT},
				.addressModeV {VK_SAMPLER_ADDRESS_MODE_REPEAT},
				.addressModeW {VK_SAMPLER_ADDRESS_MODE_REPEAT},
				.mipLodBias {0.0f},
				.anisotropyEnable {VK_FALSE},
				.maxAnisotropy {Renderer::GetConfig().maxAnisotropy},
				.compareEnable {VK_FALSE},
				.compareOp {VK_COMPARE_OP_ALWAYS},
				.minLod {0.0f},
				.maxLod {0.0f},
				.borderColor {VK_BORDER_COLOR_INT_OPAQUE_BLACK},
				.unnormalizedCoordinates {VK_FALSE}
			};

			VK_CHECK_RESULT(vkCreateSampler(device, &samplerInfo, nullptr, &mSampler));
		}

		// 
		{
			VkDescriptorSetLayoutBinding binding
			{
				.binding {0U},
				.descriptorType {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER},
				.descriptorCount {1U},
				.stageFlags {VK_SHADER_STAGE_FRAGMENT_BIT},
				.pImmutableSamplers {nullptr}
			};
			
			if (mDescriptorSet == nullptr)
			{
				VkDescriptorSetLayoutCreateInfo info
				{
					.sType {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO},
					.pNext {nullptr},
					.flags {0U},
					.bindingCount {1U},
					.pBindings {&binding}
				};
				VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &info, nullptr, &mDescriptorSetLayout));

				VkDescriptorSetAllocateInfo allocInfo
				{
					.sType {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO},
					.pNext {nullptr},
					.descriptorPool {VulkanGraphicsContext::GetDescriptorPool()},
					.descriptorSetCount {1U},
					.pSetLayouts {&mDescriptorSetLayout}
				};

				VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &mDescriptorSet));
			}

			VkDescriptorImageInfo imageInfo
			{
				.sampler {mSampler},
				.imageView {mColorAttachments[0].imageView},
				.imageLayout {VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}
			};

			std::array<VkWriteDescriptorSet, 1> descriptorWrites
			{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,		// sType
				nullptr,									// pNext
				mDescriptorSet,								// dstSet
				0,											// dstBinding
				0,											// dstArrayElement
				1,											// descriptorCount
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	// descriptorType
				&imageInfo,									// pImageInfo
				nullptr,									// pBufferInfo
				nullptr										// pTexelBufferView
			};

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}
	
	void VulkanFramebuffer::Destroy()
	{
		auto device{ VulkanGraphicsContext::GetLogicalDevice() };

		vkFreeDescriptorSets(device, VulkanGraphicsContext::GetDescriptorPool(), 1, &mDescriptorSet);
		vkDestroyDescriptorSetLayout(device, mDescriptorSetLayout, nullptr);

		for (auto colorAttachment : mColorAttachments)
		{
			VulkanAllocator::DestroyImage(colorAttachment.image);
			vkDestroyImageView(device, colorAttachment.imageView, nullptr);
		}
		if (mDepthSpecification != ImageFormat::NONE)
		{
			VulkanAllocator::DestroyImage(mDepthAttachment.image);
			vkDestroyImageView(device, mDepthAttachment.imageView, nullptr);
		}
		vkDestroyFramebuffer(device, mFramebuffer, nullptr);
		vkDestroySampler(device, mSampler, nullptr);
	}
	
	void VulkanFramebuffer::Resize(uint32_t aWidth, uint32_t aHeight)
	{
		mProperties.Width = aWidth;
		mProperties.Height = aHeight;

		auto device{ VulkanGraphicsContext::GetLogicalDevice() };
		vkDeviceWaitIdle(device);

		vkDestroyFramebuffer(device, mFramebuffer, nullptr);
		for (auto colorAttachment : mColorAttachments)
		{
			VulkanAllocator::DestroyImage(colorAttachment.image);
			vkDestroyImageView(device, colorAttachment.imageView, nullptr);
		}
		if (mDepthSpecification != ImageFormat::NONE)
		{
			VulkanAllocator::DestroyImage(mDepthAttachment.image);
			vkDestroyImageView(device, mDepthAttachment.imageView, nullptr);
		}

		Init();
	}

	uint32_t VulkanFramebuffer::ReadFramebufferPixel(uint32_t aX, uint32_t aY)
	{
		const auto& device{ VulkanGraphicsContext::GetLogicalDevice() };
		VkCommandBuffer commandBuffer{ VulkanGraphicsContext::BeginSingleCommandBuffer() };

		VkBufferCreateInfo stagingBufferInfo
		{
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			nullptr,
			0,
			sizeof(uint32_t),
			VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			0,
			nullptr
		};

		VulkanBuffer stagingBuffer;
		VmaAllocationInfo allocationInfo{};
		VulkanAllocator::AllocateBuffer(stagingBuffer, stagingBufferInfo, VMA_MEMORY_USAGE_CPU_ONLY, &allocationInfo);

		VkBufferImageCopy region;
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { (int)aX, (int)aY, 0 };
		region.imageExtent = { 1, 1, 1 };

		vkCmdCopyImageToBuffer(commandBuffer, mColorAttachments[0].image.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, stagingBuffer.buffer, 1, &region);
	
		VulkanGraphicsContext::EndSingleCommandBuffer(commandBuffer);

		uint32_t pixel{ 0U };
		void* data{ VulkanAllocator::MapMemory(stagingBuffer) };
		memcpy(&pixel, data, sizeof(uint32_t));
		VulkanAllocator::UnmapMemory(stagingBuffer);

		return pixel;
	}

	void* VulkanFramebuffer::GetID() const
	{
		return mDescriptorSet;
	}
}