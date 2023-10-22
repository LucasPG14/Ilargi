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
	VulkanRenderPass::VulkanRenderPass(const RenderPassProperties& props) : properties(props)
	{
		auto device = VulkanContext::GetLogicalDevice();
		const std::vector<ImageFormat>& formats = properties.framebuffer->GetProperties().formats;
		
		std::vector<VkAttachmentDescription> attachments;
		std::vector<VkAttachmentReference> colorAttachmentRefs;
		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_UNDEFINED;
		
		for (int i = 0; i < formats.size(); ++i)
		{
			bool isDepth = Utils::IsDepth(formats[i]);

			VkAttachmentDescription& attachment = attachments.emplace_back();
			attachment.format = Utils::GetFormatFromImageFormat(formats[i]);
			attachment.samples = VK_SAMPLE_COUNT_1_BIT;
			attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			
			attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			
			attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachment.finalLayout = isDepth ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkClearValue& clearValue = clearValues.emplace_back();

			if (!isDepth)
			{
				VkAttachmentReference& attachmentRef = colorAttachmentRefs.emplace_back();
				attachmentRef.attachment = i;
				attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };
				continue;
			}

			depthAttachmentRef.attachment = i;
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			clearValue.depthStencil = { 1.0f, 0 };
		}

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRefs.size());
		subpass.pColorAttachments = colorAttachmentRefs.data();
		subpass.pDepthStencilAttachment = depthAttachmentRef.layout == VK_IMAGE_LAYOUT_UNDEFINED ? nullptr : &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass));

		std::static_pointer_cast<VulkanFramebuffer>(props.framebuffer)->Init(renderPass);
		std::static_pointer_cast<VulkanPipeline>(props.pipeline)->Init(renderPass, formats);
	}
	
	VulkanRenderPass::~VulkanRenderPass()
	{
	}

	void VulkanRenderPass::Destroy()
	{
		auto device = VulkanContext::GetLogicalDevice();

		properties.pipeline->Destroy();
		vkDestroyRenderPass(device, renderPass, nullptr);
	}
	
	void VulkanRenderPass::BeginRenderPass(const std::shared_ptr<CommandBuffer>& commandBuffer) const
	{
		Renderer::Submit([this, commandBuffer]()
			{
				auto framebuffer = std::static_pointer_cast<VulkanFramebuffer>(properties.framebuffer);
				auto cmdBuffer = std::static_pointer_cast<VulkanCommandBuffer>(commandBuffer)->GetCurrentCommand(Renderer::GetCurrentFrame());

				VkRenderPassBeginInfo renderPassInfo = {};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderPass = renderPass;
				renderPassInfo.framebuffer = framebuffer->GetFramebuffer();

				uint32_t width = framebuffer->GetWidth();
				uint32_t height = framebuffer->GetHeight();

				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = { width, height };

				renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
				renderPassInfo.pClearValues = clearValues.data();

				VkViewport viewport{};
				viewport.x = 0.0f;
				viewport.y = 0.0f;
				viewport.width = (float)width;
				viewport.height = (float)height;
				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;
				vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

				VkRect2D scissor{};
				scissor.offset = { 0, 0 };
				scissor.extent = { width, height };
				vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

				vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			});
	}

	void VulkanRenderPass::EndRenderPass(const std::shared_ptr<CommandBuffer>& commandBuffer) const
	{
		Renderer::Submit([commandBuffer]()
			{
				auto cmdBuffer = std::static_pointer_cast<VulkanCommandBuffer>(commandBuffer)->GetCurrentCommand(Renderer::GetCurrentFrame());
				vkCmdEndRenderPass(cmdBuffer);
			});
	}
}