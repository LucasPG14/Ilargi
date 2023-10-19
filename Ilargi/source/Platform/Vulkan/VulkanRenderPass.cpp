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

		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = Utils::GetFormatFromImageFormat(properties.framebuffer->GetProperties().formats[0]);
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = VK_FORMAT_D32_SFLOAT;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
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
		std::static_pointer_cast<VulkanPipeline>(props.pipeline)->Init(renderPass);
	}
	
	VulkanRenderPass::~VulkanRenderPass()
	{
	}

	void VulkanRenderPass::Destroy()
	{
		auto device = VulkanContext::GetLogicalDevice();

		vkDestroyRenderPass(device, renderPass, nullptr);
	}
	
	void VulkanRenderPass::BeginRenderPass(std::shared_ptr<CommandBuffer> commandBuffer) const
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

				std::array<VkClearValue, 2> clearValues = {};
				clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
				clearValues[1].depthStencil = { 1.0f, 0 };

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

	void VulkanRenderPass::EndRenderPass(std::shared_ptr<CommandBuffer> commandBuffer) const
	{
		Renderer::Submit([commandBuffer]()
			{
				auto cmdBuffer = std::static_pointer_cast<VulkanCommandBuffer>(commandBuffer)->GetCurrentCommand(Renderer::GetCurrentFrame());
				vkCmdEndRenderPass(cmdBuffer);
			});
	}
	
	void VulkanRenderPass::PushConstants(std::shared_ptr<CommandBuffer> commandBuffer, void* data) const
	{
		Renderer::Submit([this, commandBuffer, data]()
		{
			//properties.pipeline->PushConstants(commandBuffer, data);
		});
	}
}