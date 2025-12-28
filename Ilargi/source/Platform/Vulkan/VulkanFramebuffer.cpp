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

		const auto& renderPass{ mProperties.renderPass->As<VulkanRenderPass>() };
		Init(renderPass->GetRenderPass());
	}
	
	VulkanFramebuffer::~VulkanFramebuffer()
	{
	}

	void VulkanFramebuffer::Init(VkRenderPass aRenderPass)
	{
		auto device{ VulkanContext::GetLogicalDevice() };
		
		std::vector<VkImageView> attachments;

		uint32_t maxSamples{ Renderer::GetConfig().maxAASamples };

		mColorAttachments.resize(mColorSpecifications.size());
		int i{ 0 };
		for (VulkanAttachment& attachment : mColorAttachments)
		{
			VkFormat format{ Utils::GetFormatFromImageFormat(mColorSpecifications[i++]) };

			VkImageCreateInfo imageInfo
			{
				VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,								// sType
				nullptr,															// pNext
				0,																	// flags
				VK_IMAGE_TYPE_2D,													// imageType
				format,																// format
				{mProperties.width, mProperties.height, 1},							// extent
				1,																	// mipLevels
				1,																	// arrayLayers
				VK_SAMPLE_COUNT_1_BIT,												// samples
				VK_IMAGE_TILING_OPTIMAL,											// tiling
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,	// usage
				VK_SHARING_MODE_EXCLUSIVE,											// sharingMode
				0,																	// queueFamilyIndexCount
				nullptr,															// pQueueFamilyIndices
				VK_IMAGE_LAYOUT_UNDEFINED											// initialLayout
			};

			VulkanAllocator::AllocateImage(attachment.image, imageInfo, VMA_MEMORY_USAGE_GPU_ONLY, "Framebuffer");

			VkImageViewCreateInfo imageViewInfo
			{
				VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,	// sType
				nullptr,									// pNext
				0,											// flags
				attachment.image.image,						// image
				VK_IMAGE_VIEW_TYPE_2D,						// viewType
				format,										// format
				{VK_COMPONENT_SWIZZLE_IDENTITY},			// components: RGBA
				{											// subresourceRange:
					VK_IMAGE_ASPECT_COLOR_BIT,					// aspectMask
					0,											// baseMipLevel
					1,											// levelCount
					0,											// baseArrayLayer
					1											// layerCount
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
				VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,					// sType
				nullptr,												// pNext
				0,														// flags
				VK_IMAGE_TYPE_2D,										// imageType
				depthFormat,											// format
				{mProperties.width, mProperties.height, 1},				// extent
				1,														// mipLevels
				1,														// arrayLayers
				VK_SAMPLE_COUNT_1_BIT,									// samples
				VK_IMAGE_TILING_OPTIMAL,								// tiling
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,			// usage
				VK_SHARING_MODE_EXCLUSIVE,								// sharingMode
				0,														// queueFamilyIndexCount
				nullptr,												// pQueueFamilyIndices
				VK_IMAGE_LAYOUT_UNDEFINED								// initialLayout
			};

			VulkanAllocator::AllocateImage(mDepthAttachment.image, imageInfo, VMA_MEMORY_USAGE_GPU_ONLY, "Framebuffer");
			
			VkImageViewCreateInfo imageViewInfo
			{
				VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,	// sType
				nullptr,									// pNext
				0,											// flags
				mDepthAttachment.image.image,				// image
				VK_IMAGE_VIEW_TYPE_2D,						// viewType
				depthFormat,								// format
				{VK_COMPONENT_SWIZZLE_IDENTITY},			// components: RGBA
				{											// subresourceRange:
					VK_IMAGE_ASPECT_DEPTH_BIT,					// aspectMask
					0,											// baseMipLevel
					1,											// levelCount
					0,											// baseArrayLayer
					1											// layerCount
				}
			};
			
			VK_CHECK_RESULT(vkCreateImageView(device, &imageViewInfo, nullptr, &mDepthAttachment.imageView));

			attachments.push_back(mDepthAttachment.imageView);
		}

		// Creating the framebuffer
		{
			VkFramebufferCreateInfo framebufferInfo
			{
				VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,	// sType
				nullptr,									// pNext
				0,											// flags
				aRenderPass,								// renderPass
				static_cast<uint32_t>(attachments.size()),	// attachmentCount
				attachments.data(),							// pAttachments
				mProperties.width,							// width
				mProperties.height,							// height
				1											// layers
			};

			VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &mFramebuffer));
		}

		// Sampler
		if (!mSampler) 
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
				Renderer::GetConfig().maxAnisotropy,	// maxAnisotropy
				VK_FALSE,								// compareEnable
				VK_COMPARE_OP_ALWAYS,					// compareOp
				0.0f,									// minLod
				0.0f,									// maxLod
				VK_BORDER_COLOR_INT_OPAQUE_BLACK,		// borderColor
				VK_FALSE								// unnormalizedCoordinates
			};

			VK_CHECK_RESULT(vkCreateSampler(device, &samplerInfo, nullptr, &mSampler));
		}

		// 
		{
			VkDescriptorSetLayoutBinding binding[1]
			{
				0,											// binding
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	// descriptorType
				1,											// descriptorCount
				VK_SHADER_STAGE_FRAGMENT_BIT,				// stageFlags
				nullptr										// pImmutableSamplers
			};
			
			VkDescriptorSetLayoutCreateInfo info
			{
				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,	// sType
				nullptr,												// pNext
				0,														// flags
				1,														// bindingCount
				binding													// pBindings
			};
			VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &info, nullptr, &mDescriptorSetLayout));

			VkDescriptorSetAllocateInfo allocInfo
			{
				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,	// sType
				nullptr,										// pNext
				VulkanContext::GetDescriptorPool(),				// descriptorPool
				1,												// descriptorSetCount
				&mDescriptorSetLayout							// pSetLayouts
			};

			VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &mDescriptorSet));

			VkDescriptorImageInfo imageInfo
			{
				mSampler,									// sampler
				mColorAttachments[0].imageView,				// imageView
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL	// imageLayout
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
		
			vkDestroyDescriptorSetLayout(device, mDescriptorSetLayout, nullptr);
		}
	}
	
	void VulkanFramebuffer::Destroy()
	{
		auto device{ VulkanContext::GetLogicalDevice() };

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

		auto device{ VulkanContext::GetLogicalDevice() };
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

		Init(aRenderPass->As<VulkanRenderPass>()->GetRenderPass());
	}

	void* VulkanFramebuffer::GetID() const
	{
		return mDescriptorSet;
	}
}