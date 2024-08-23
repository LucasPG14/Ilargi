#include "ilargipch.h"

// Main headers
#include "VulkanFramebuffer.h"
#include "Renderer/Renderer.h"
#include "VulkanContext.h"
#include "VulkanRenderPass.h"

namespace Ilargi
{
	VulkanFramebuffer::VulkanFramebuffer(const FramebufferProperties& aProperties) 
		: mProperties(aProperties), mDepthSpecification(ImageFormat::NONE), mDepthAttachment(), mFramebuffer(VK_NULL_HANDLE), 
		mSampler(VK_NULL_HANDLE), mDescriptorSetLayout(VK_NULL_HANDLE), mDescriptorSet(VK_NULL_HANDLE)
	{
		for (ImageFormat format : aProperties.formats)
		{
			if (Utils::IsDepth(format))
			{
				mDepthSpecification = format; 
				continue;
			}
			mColorSpecifications.push_back(format);
		}
	}
	
	VulkanFramebuffer::~VulkanFramebuffer()
	{
	}

	void VulkanFramebuffer::Init(VkRenderPass aRenderPass)
	{
		auto device = VulkanContext::GetLogicalDevice();
		
		std::vector<VkImageView> attachments;

		uint32_t maxSamples = Renderer::GetConfig().maxAASamples;

		mColorAttachments.resize(mColorSpecifications.size());
		int i = 0;
		for (VulkanAttachment& attachment : mColorAttachments)
		{
			VkFormat format = Utils::GetFormatFromImageFormat(mColorSpecifications[i++]);

			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = mProperties.width;
			imageInfo.extent.height = mProperties.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = format;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.flags = 0;

			VulkanAllocator::AllocateImage(attachment.image, imageInfo, VMA_MEMORY_USAGE_GPU_ONLY);

			VkImageViewCreateInfo imageViewInfo = {};
			imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewInfo.image = attachment.image.image;

			imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewInfo.format = format;

			imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewInfo.subresourceRange.baseMipLevel = 0;
			imageViewInfo.subresourceRange.levelCount = 1;
			imageViewInfo.subresourceRange.baseArrayLayer = 0;
			imageViewInfo.subresourceRange.layerCount = 1;

			VK_CHECK_RESULT(vkCreateImageView(device, &imageViewInfo, nullptr, &attachment.imageView));

			attachments.push_back(attachment.imageView);
		}

		// Creating the depth image
		if (mDepthSpecification != ImageFormat::NONE)
		{
			VkFormat depthFormat = Utils::GetFormatFromImageFormat(mDepthSpecification);
			
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = mProperties.width;
			imageInfo.extent.height = mProperties.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = depthFormat;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.flags = 0;

			VulkanAllocator::AllocateImage(mDepthAttachment.image, imageInfo, VMA_MEMORY_USAGE_GPU_ONLY);
			
			VkImageViewCreateInfo imageViewInfo = {};
			imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewInfo.image = mDepthAttachment.image.image;

			imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewInfo.format = depthFormat;

			imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			imageViewInfo.subresourceRange.baseMipLevel = 0;
			imageViewInfo.subresourceRange.levelCount = 1;
			imageViewInfo.subresourceRange.baseArrayLayer = 0;
			imageViewInfo.subresourceRange.layerCount = 1;
			
			VK_CHECK_RESULT(vkCreateImageView(device, &imageViewInfo, nullptr, &mDepthAttachment.imageView));

			attachments.push_back(mDepthAttachment.imageView);
		}

		// Creating the framebuffer
		{
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = aRenderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = mProperties.width;
			framebufferInfo.height = mProperties.height;
			framebufferInfo.layers = 1;

			VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &mFramebuffer));
		}

		// Sampler
		if (!mSampler) 
		{
			VkSamplerCreateInfo samplerInfo{};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = VK_FILTER_LINEAR;
			samplerInfo.minFilter = VK_FILTER_LINEAR;

			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

			samplerInfo.anisotropyEnable = VK_FALSE;
			samplerInfo.maxAnisotropy = Renderer::GetConfig().maxAnisotropy;
			samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			samplerInfo.unnormalizedCoordinates = VK_FALSE;
			samplerInfo.compareEnable = VK_FALSE;
			samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.mipLodBias = 0.0f;
			samplerInfo.minLod = 0.0f;
			samplerInfo.maxLod = 0.0f;

			VK_CHECK_RESULT(vkCreateSampler(device, &samplerInfo, nullptr, &mSampler));
		}

		// 
		{
			VkDescriptorSetLayoutBinding binding[1] = {};
			binding[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			binding[0].descriptorCount = 1;
			binding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			
			VkDescriptorSetLayoutCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			info.bindingCount = 1;
			info.pBindings = binding;
			VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &info, nullptr, &mDescriptorSetLayout));

			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = VulkanContext::GetDescriptorPool();
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &mDescriptorSetLayout;

			VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &mDescriptorSet));

			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = mColorAttachments[0].imageView;
			imageInfo.sampler = mSampler;

			std::array<VkWriteDescriptorSet, 1> descriptorWrites = {};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = mDescriptorSet;
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		
			vkDestroyDescriptorSetLayout(device, mDescriptorSetLayout, nullptr);
		}
	}
	
	void VulkanFramebuffer::Destroy()
	{
		auto device = VulkanContext::GetLogicalDevice();

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
	
	void VulkanFramebuffer::Resize(const std::shared_ptr<RenderPass>& aRenderPass, uint32_t aWidth, uint32_t aHeight)
	{
		mProperties.width = aWidth;
		mProperties.height = aHeight;

		auto device = VulkanContext::GetLogicalDevice();
		vkDeviceWaitIdle(device);

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

		Init(std::static_pointer_cast<VulkanRenderPass>(aRenderPass)->GetRenderPass());
	}

	void* VulkanFramebuffer::GetID() const
	{
		return mDescriptorSet;
	}
}