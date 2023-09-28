#pragma once

#include "Renderer/RenderPass.h"

#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanRenderPass : public RenderPass
	{
	public:
		VulkanRenderPass(const RenderPassProperties& props);
		virtual ~VulkanRenderPass();

		void Destroy() override;

		void BeginRenderPass(std::shared_ptr<CommandBuffer> commandBuffer) const override;
		void BeginRenderPass(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, uint32_t width, uint32_t height) const;

		const RenderPassProperties& GetProperties() const override { return properties; }

		const VkRenderPass GetRenderPass() const { return renderPass; }
	private:
		RenderPassProperties properties;

		VkRenderPass renderPass;
	};
}