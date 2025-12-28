#include "ilargipch.h"

// Main headers
#include "VulkanRenderPass.h"
#include "Renderer/Renderer.h"
#include "Renderer/Pipeline.h"
#include "VulkanFramebuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanPipeline.h"
#include "VulkanContext.h"
#include "VulkanUtils.h"

namespace Ilargi
{
	VulkanRenderPass::VulkanRenderPass(const RenderPassProperties& props) : mProperties(props)
	{
		auto device{ VulkanContext::GetLogicalDevice() };
		const std::vector<ImageFormat>& formats{ mProperties.formats };
		
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
			
			attachment.stencilLoadOp = mProperties.clearValues ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : VK_ATTACHMENT_LOAD_OP_LOAD;
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
			0,																							// flags
			VK_PIPELINE_BIND_POINT_GRAPHICS,															// sType
			0,																							// inputAttachmentCount
			nullptr,																					// pInputAttachments
			static_cast<uint32_t>(colorAttachmentRefs.size()),											// colorAttachmentCount
			colorAttachmentRefs.data(),																	// pColorAttachments
			nullptr,																					// pResolveAttachments
			depthAttachmentRef.layout == VK_IMAGE_LAYOUT_UNDEFINED ? nullptr : &depthAttachmentRef,		// pDepthStencilAttachment
			0,																							// preserveAttachmentCount
			nullptr																						// pPreserveAttachments
		};

		VkSubpassDependency dependency
		{
			VK_SUBPASS_EXTERNAL,																		// srcSubpass
			0,																							// dstSubpass
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, // srcStageMask
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, // dstStageMask
			VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,			// srcAccessMask
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT			// dstAccessMask
		};

		VkRenderPassCreateInfo renderPassInfo
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,	// sType
			nullptr,									// pNext
			0,											// flags
			static_cast<uint32_t>(attachments.size()),	// attachmentCount
			attachments.data(),							// pAttachments
			1,											// subpassCount
			&subpass,									// pSubpasses
			1,											// dependencyCount
			&dependency									// pDependencies
		};

		VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassInfo, nullptr, &mRenderPass));

		//props.framebuffer->As<VulkanFramebuffer>()->Init(mRenderPass);
		//props.pipeline->As<VulkanPipeline>()->Init(mRenderPass, formats);
	}
	
	VulkanRenderPass::~VulkanRenderPass()
	{
	}

	void VulkanRenderPass::Destroy()
	{
		auto device{ VulkanContext::GetLogicalDevice() };

		//mProperties.pipeline->Destroy();
		vkDestroyRenderPass(device, mRenderPass, nullptr);
	}
	
	void VulkanRenderPass::BeginRenderPass(const std::shared_ptr<CommandBuffer>& aCommandBuffer, const std::shared_ptr<Framebuffer>& aFramebuffer) const
	{
		Renderer::Submit([this, aCommandBuffer, aFramebuffer]()
			{
				auto framebuffer{ aFramebuffer->As<VulkanFramebuffer>() };
				auto cmdBuffer{ aCommandBuffer->As<VulkanCommandBuffer>()->GetCurrentCommand(Renderer::GetCurrentFrame())};

				uint32_t width{ framebuffer->GetWidth() };
				uint32_t height{ framebuffer->GetHeight() };

				VkRenderPassBeginInfo renderPassInfo
				{
					VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,	// sType
					nullptr,									// pNext
					mRenderPass,								// renderPass
					framebuffer->GetFramebuffer(),				// framebuffer
					{											// renderArea
						{ 0, 0 },									// offset
						{ width, height }							// extent
					},
					mProperties.clearValues ? static_cast<uint32_t>(mClearValues.size()) : 0,	// clearValueCount 
					mClearValues.data()							// pClearValues 
				};

				VkViewport viewport
				{
					0.0f,			// x
					0.0f,			// y
					(float)width,	// width
					(float)height,	// height
					0.0f,			// minDepth
					1.0f			// maxDepth
				};
				vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

				VkRect2D scissor
				{
					{ 0, 0 },			// offset
					{ width, height }	// extent
				};
				vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

				vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			});
	}

	void VulkanRenderPass::EndRenderPass(const std::shared_ptr<CommandBuffer>& aCommandBuffer) const
	{
		Renderer::Submit([aCommandBuffer]()
			{
				auto cmdBuffer{ aCommandBuffer->As<VulkanCommandBuffer>()->GetCurrentCommand(Renderer::GetCurrentFrame()) };
				vkCmdEndRenderPass(cmdBuffer);
			});
	}
}