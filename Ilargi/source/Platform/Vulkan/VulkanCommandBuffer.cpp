#include "ilargipch.h"
#include "VulkanCommandBuffer.h"

#include "Renderer/Renderer.h"
#include "VulkanGraphicsContext.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanMaterial.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanShader.h"
#include "VulkanUniformBuffer.h"

namespace Ilargi
{
	VulkanCommandBuffer::VulkanCommandBuffer(uint32_t aFramesInFlight) : mQueryPoolCount(aFramesInFlight * 2)
	{
		auto device{ VulkanGraphicsContext::GetLogicalDevice() };

		mCommandBuffers.resize(aFramesInFlight);

		VkCommandBufferAllocateInfo allocInfo
		{
			.sType {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO},
			.pNext {nullptr},
			.commandPool {VulkanGraphicsContext::GetCommandPool()},
			.level {VK_COMMAND_BUFFER_LEVEL_PRIMARY},
			.commandBufferCount {static_cast<uint32_t>(mCommandBuffers.size())}
		};

		VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &allocInfo, mCommandBuffers.data()));

		// TODO: Remove
		VkFenceCreateInfo fenceInfo
		{
			VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,	// sType
			nullptr,								// pNext
			VK_FENCE_CREATE_SIGNALED_BIT			// flags
		};

		VK_CHECK_RESULT(vkCreateFence(device, &fenceInfo, nullptr, &mFence));
	}
	
	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
	}

	void VulkanCommandBuffer::Destroy() const
	{
		auto device{ VulkanGraphicsContext::GetLogicalDevice() };

		vkDestroyFence(device, mFence, nullptr);
	}

	void VulkanCommandBuffer::BeginCommand() const
	{
		Renderer::Submit([this]()
			{
				uint32_t currentFrame{ Renderer::GetCurrentFrame() };

				VkCommandBufferBeginInfo beginInfo
				{
					VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,	// sType
					nullptr,										// pNext		
					0,												// flags
					nullptr											// pInheritanceInfo
				};

				VK_CHECK_RESULT(vkBeginCommandBuffer(mCommandBuffers[currentFrame], &beginInfo));
			});
	}
	
	void VulkanCommandBuffer::EndCommand() const
	{
		Renderer::Submit([this]()
			{
				uint32_t currentFrame{ Renderer::GetCurrentFrame() };

				VK_CHECK_RESULT(vkEndCommandBuffer(mCommandBuffers[currentFrame]));
			});
	}

	void VulkanCommandBuffer::BeginRenderPass(const IRenderPass& aRenderPass, const std::shared_ptr<IFramebuffer>& aFramebuffer) const
	{
		VkRenderPass renderPass{ aRenderPass.As<VulkanRenderPass>().GetRenderPass() };
		Renderer::Submit([this, renderPass, aFramebuffer]()
			{
				const std::shared_ptr<VulkanFramebuffer>& framebuffer{ aFramebuffer->As<VulkanFramebuffer>() };			

				uint32_t width{ framebuffer->GetWidth() };
				uint32_t height{ framebuffer->GetHeight() };

				// TODO: ClearValues based on formats
				VkClearValue aValue{};
				std::vector<VkClearValue> clearValues;
				for (const auto& format : aFramebuffer->GetColorSpecifications())
				{
					VkClearValue clearValue;
					clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };
					clearValues.push_back(clearValue);
				}

				if (aFramebuffer->GetDepthSpecification() != ImageFormat::NONE)
				{
					VkClearValue clearValue;
					clearValue.depthStencil = { 1.0f, 0 };
					clearValues.push_back(clearValue);
				}

				VkRenderPassBeginInfo renderPassInfo
				{
					VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,	// sType
					nullptr,									// pNext
					renderPass,									// renderPass
					framebuffer->GetFramebuffer(),				// framebuffer
					{											// renderArea
						{ 0, 0 },									// offset
						{ width, height }							// extent
					},
					clearValues.size(),							// clearValueCount 
					clearValues.data()							// pClearValues 
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
				vkCmdSetViewport(mCommandBuffers[Renderer::GetCurrentFrame()], 0, 1, &viewport);

				VkRect2D scissor
				{
					{ 0, 0 },			// offset
					{ width, height }	// extent
				};
				vkCmdSetScissor(mCommandBuffers[Renderer::GetCurrentFrame()], 0, 1, &scissor);

				vkCmdBeginRenderPass(mCommandBuffers[Renderer::GetCurrentFrame()], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			});
	}

	void VulkanCommandBuffer::EndRenderPass() const
	{
		Renderer::Submit([this]()
			{
				vkCmdEndRenderPass(mCommandBuffers[Renderer::GetCurrentFrame()]);
			});
	}

	void VulkanCommandBuffer::BindPipeline(const std::shared_ptr<IGraphicsPipeline>& aPipeline) const
	{
		Renderer::Submit([this, aPipeline]()
			{
				const std::shared_ptr<VulkanGraphicsPipeline>& pipeline{ aPipeline->As<VulkanGraphicsPipeline>() };

				vkCmdBindPipeline(mCommandBuffers[Renderer::GetCurrentFrame()], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());
			});
	}

	void VulkanCommandBuffer::PushConstants(std::string_view aShaderName, uint32_t aOffset, uint32_t aSize, const void* aData) const
	{
		Renderer::Submit([this, aShaderName, aOffset, aSize, aData]()
			{
				const uint32_t currentFrame{ Renderer::GetCurrentFrame() };

				vkCmdPushConstants(mCommandBuffers[currentFrame], Renderer::GetShader(aShaderName.data())->As<VulkanShader>()->GetPipelineLayout(), VK_SHADER_STAGE_ALL_GRAPHICS, aOffset, aSize, aData);
			});
	}

	void VulkanCommandBuffer::BindMaterial(std::string_view aShaderName, const std::shared_ptr<Material>& aMaterial, uint32_t aSetIndex) const
	{
		Renderer::Submit([this, aShaderName, aMaterial, aSetIndex]()
			{
				uint32_t currentFrame{ Renderer::GetCurrentFrame() };

				std::vector<VkDescriptorSet> descriptorSets{ aMaterial->As<VulkanMaterial>()->GetDescriptorSet() };
				vkCmdBindDescriptorSets(mCommandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::GetShader(aShaderName.data())->As<VulkanShader>()->GetPipelineLayout(), aSetIndex, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
			});
	}

	void VulkanCommandBuffer::BindUniformBuffer(std::string_view aShaderName, const std::shared_ptr<IUniformBuffer>& aUniformBuffer, uint32_t aSetIndex) const
	{
		Renderer::Submit([this, aShaderName, aUniformBuffer, aSetIndex]()
			{
				uint32_t currentFrame{ Renderer::GetCurrentFrame() };

				std::vector<VkDescriptorSet> descriptorSets{ aUniformBuffer->As<VulkanUniformBuffer>()->GetDescriptorSet() };
				vkCmdBindDescriptorSets(mCommandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::GetShader(aShaderName.data())->As<VulkanShader>()->GetPipelineLayout(), aSetIndex, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
			});
	}
	
	void VulkanCommandBuffer::Submit() const
	{
		Renderer::Submit([this]()
			{
				uint32_t currentFrame{ Renderer::GetCurrentFrame() };
				auto device{ VulkanGraphicsContext::GetLogicalDevice() };
				
				VkPipelineStageFlags waitStages[] { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

				VkSubmitInfo submitInfo
				{
					VK_STRUCTURE_TYPE_SUBMIT_INFO,	// sType
					nullptr,						// pNext								
					0,								// waitSemaphoreCount
					nullptr,						// pWaitSemaphores
					waitStages,						// pWaitDstStageMask
					1,								// commandBufferCount
					&mCommandBuffers[currentFrame], // pCommandBuffers
					0,								// signalSemaphoreCount
					nullptr							// pSignalSemaphores
				};

				VK_CHECK_RESULT(vkResetFences(device, 1, &mFence));
				VK_CHECK_RESULT(vkQueueSubmit(VulkanGraphicsContext::GetGraphicsQueue(), 1, &submitInfo, mFence));
				
				VK_CHECK_RESULT(vkWaitForFences(device, 1, &mFence, VK_TRUE, UINT64_MAX));
			});
	}
}