#include "ilargipch.h"

// Main headers
#include "VulkanFramebuffer.h"
#include "Renderer/Renderer.h"
#include "VulkanContext.h"
#include "VulkanRenderPass.h"

namespace Ilargi
{
	VulkanFramebuffer::VulkanFramebuffer(const FramebufferProperties& props) 
		: properties(props), depthSpecification(ImageFormat::NONE), framebuffer(VK_NULL_HANDLE), 
		sampler(VK_NULL_HANDLE), descriptorSetLayout(VK_NULL_HANDLE), descriptorSet(VK_NULL_HANDLE)
	{
		for (ImageFormat format : props.formats)
		{
			if (Utils::IsDepth(format))
			{
				depthSpecification = format; 
				continue;
			}
			colorSpecifications.push_back(format);
		}
	}
	
	VulkanFramebuffer::~VulkanFramebuffer()
	{
	}

	void VulkanFramebuffer::Init(VkRenderPass renderPass)
	{
		auto device = VulkanContext::GetLogicalDevice();
		
		std::vector<VkImageView> attachments;

		colorAttachments.resize(colorSpecifications.size());
		int i = 0;
		for (VulkanAttachment& attachment : colorAttachments)
		{
			VkFormat format = Utils::GetFormatFromImageFormat(colorSpecifications[i++]);

			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = properties.width;
			imageInfo.extent.height = properties.height;
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
		if (depthSpecification != ImageFormat::NONE)
		{
			VkFormat depthFormat = Utils::GetFormatFromImageFormat(depthSpecification);
			
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = properties.width;
			imageInfo.extent.height = properties.height;
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

			VulkanAllocator::AllocateImage(depthAttachment.image, imageInfo, VMA_MEMORY_USAGE_GPU_ONLY);
			
			VkImageViewCreateInfo imageViewInfo = {};
			imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewInfo.image = depthAttachment.image.image;

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
			
			VK_CHECK_RESULT(vkCreateImageView(device, &imageViewInfo, nullptr, &depthAttachment.imageView));

			attachments.push_back(depthAttachment.imageView);
		}

		// Creating the framebuffer
		{
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = properties.width;
			framebufferInfo.height = properties.height;
			framebufferInfo.layers = 1;

			VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer));
		}

		// Sampler
		if (!sampler) 
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

			VK_CHECK_RESULT(vkCreateSampler(device, &samplerInfo, nullptr, &sampler));
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
			VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &info, nullptr, &descriptorSetLayout));

			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = VulkanContext::GetDescriptorPool();
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = &descriptorSetLayout;

			VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet));

			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = colorAttachments[0].imageView;
			imageInfo.sampler = sampler;

			std::array<VkWriteDescriptorSet, 1> descriptorWrites = {};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = descriptorSet;
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		
			vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
		}
	}
	
	void VulkanFramebuffer::Destroy()
	{
		auto device = VulkanContext::GetLogicalDevice();

		for (auto colorAttachment : colorAttachments)
		{
			VulkanAllocator::DestroyImage(colorAttachment.image);
			vkDestroyImageView(device, colorAttachment.imageView, nullptr);
		}
		if (depthSpecification != ImageFormat::NONE)
		{
			VulkanAllocator::DestroyImage(depthAttachment.image);
			vkDestroyImageView(device, depthAttachment.imageView, nullptr);
		}
		vkDestroyFramebuffer(device, framebuffer, nullptr);
		vkDestroySampler(device, sampler, nullptr);
	}
	
	void VulkanFramebuffer::Resize(const std::shared_ptr<RenderPass>& renderPass, uint32_t width, uint32_t height)
	{
		properties.width = width;
		properties.height = height;

		auto device = VulkanContext::GetLogicalDevice();
		vkDeviceWaitIdle(device);

		for (auto colorAttachment : colorAttachments)
		{
			VulkanAllocator::DestroyImage(colorAttachment.image);
			vkDestroyImageView(device, colorAttachment.imageView, nullptr);
		}
		if (depthSpecification != ImageFormat::NONE)
		{
			VulkanAllocator::DestroyImage(depthAttachment.image);
			vkDestroyImageView(device, depthAttachment.imageView, nullptr);
		}
		vkDestroyFramebuffer(device, framebuffer, nullptr);

		Init(std::static_pointer_cast<VulkanRenderPass>(renderPass)->GetRenderPass());
	}

	void* VulkanFramebuffer::GetID() const
	{
		return descriptorSet;
	}
}