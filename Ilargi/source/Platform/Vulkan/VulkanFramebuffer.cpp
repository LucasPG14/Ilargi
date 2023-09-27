#include "ilargipch.h"

// Main headers
#include "VulkanFramebuffer.h"
#include "VulkanContext.h"

namespace Ilargi
{
	VulkanFramebuffer::VulkanFramebuffer(const FramebufferProperties& props) 
		: properties(props), image(VK_NULL_HANDLE), imageView(VK_NULL_HANDLE), framebuffer(VK_NULL_HANDLE)
	{
	}
	
	VulkanFramebuffer::~VulkanFramebuffer()
	{
	}

	void VulkanFramebuffer::Init(VkRenderPass renderPass)
	{
		auto device = VulkanContext::GetLogicalDevice();

		// Creating the image
		{

		}

		// Creating image view
		{
			VkImageViewCreateInfo imageViewInfo = {};
			imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewInfo.image = image;

			imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;

			imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewInfo.subresourceRange.baseMipLevel = 0;
			imageViewInfo.subresourceRange.levelCount = 1;
			imageViewInfo.subresourceRange.baseArrayLayer = 0;
			imageViewInfo.subresourceRange.layerCount = 1;

			VK_CHECK_RESULT(vkCreateImageView(device, &imageViewInfo, nullptr, &imageView) == VK_SUCCESS,
				"Unable to create image view");
		}

		// Creating the framebuffer
		{
			std::array<VkImageView, 1> attachments = { imageView };

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = properties.width;
			framebufferInfo.height = properties.height;
			framebufferInfo.layers = 1;

			VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) == VK_SUCCESS,
				"Unable to create the framebuffer");
		}
	}
}