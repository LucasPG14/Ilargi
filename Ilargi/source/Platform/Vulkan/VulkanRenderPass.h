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
		void EndRenderPass(std::shared_ptr<CommandBuffer> commandBuffer) const override;

		void PushConstants(std::shared_ptr<CommandBuffer> commandBuffer, void* data) const override;

		const RenderPassProperties& GetProperties() const override { return properties; }

		const VkRenderPass GetRenderPass() const { return renderPass; }
	private:
		RenderPassProperties properties;

		VkRenderPass renderPass;
	};
}