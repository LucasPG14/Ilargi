#include "ilargipch.h"

// Main headers
#include "VulkanRenderPass.h"
#include "Renderer/Renderer.h"
#include "VulkanFramebuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanGraphicsContext.h"
#include "VulkanUtils.h"

namespace Ilargi
{
	VulkanRenderPass::VulkanRenderPass(const RenderPassProperties& props) : mProperties(props)
	{
		const VkDevice& device{ VulkanGraphicsContext::GetLogicalDevice() };
		const std::vector<ImageFormat>& formats{ mProperties.Formats };
		
		std::vector<VkAttachmentDescription> attachments;
		std::vector<VkAttachmentReference> colorAttachmentRefs;
		VkAttachmentReference depthAttachmentRef {};
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_UNDEFINED;
		
		for (uint32_t i { 0 }; i < formats.size(); ++i)
		{
			bool isDepth{ Utils::IsDepth(formats[i]) };

			VkAttachmentDescription& attachment{ attachments.emplace_back() };
			attachment.format = Utils::GetFormatFromImageFormat(formats[i]);
			attachment.samples = VK_SAMPLE_COUNT_1_BIT;
			attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			
			attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
			
			attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachment.finalLayout = isDepth ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkClearValue& clearValue{ mClearValues.emplace_back() };

			if (!isDepth)
			{
				VkAttachmentReference& attachmentRef{ colorAttachmentRefs.emplace_back() };
				attachmentRef.attachment = i;
				attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };
				continue;
			}

			depthAttachmentRef.attachment = i;
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			clearValue.depthStencil = { 1.0f, 0 };
		}

		VkSubpassDescription subpass 
		{
			.flags {0U},
			.pipelineBindPoint {VK_PIPELINE_BIND_POINT_GRAPHICS},
			.inputAttachmentCount {0U},
			.pInputAttachments {nullptr},
			.colorAttachmentCount {static_cast<uint32_t>(colorAttachmentRefs.size())},
			.pColorAttachments {colorAttachmentRefs.data()},
			.pResolveAttachments {nullptr},
			.pDepthStencilAttachment {depthAttachmentRef.layout == VK_IMAGE_LAYOUT_UNDEFINED ? nullptr : &depthAttachmentRef},
			.preserveAttachmentCount {0U},
			.pPreserveAttachments {nullptr}
		};

		VkSubpassDependency dependency
		{
			.srcSubpass {VK_SUBPASS_EXTERNAL},
			.dstSubpass {0U},
			.srcStageMask {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT},
			.dstStageMask {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT},
			.srcAccessMask {VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT},
			.dstAccessMask {VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT}
		};

		VkRenderPassCreateInfo renderPassInfo
		{
			.sType {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO},
			.pNext {nullptr},
			.flags {0U},
			.attachmentCount {static_cast<uint32_t>(attachments.size())},
			.pAttachments {attachments.data()},
			.subpassCount {1U},
			.pSubpasses {&subpass},
			.dependencyCount {1U},
			.pDependencies {&dependency}
		};

		VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassInfo, nullptr, &mRenderPass));
	}
	
	VulkanRenderPass::~VulkanRenderPass()
	{
		const VkDevice& device{ VulkanGraphicsContext::GetLogicalDevice() };
		vkDestroyRenderPass(device, mRenderPass, nullptr);
	}

	void VulkanRenderPass::Destroy()
	{
	}
}