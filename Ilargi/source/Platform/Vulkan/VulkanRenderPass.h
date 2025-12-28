#pragma once

#include "Renderer/RenderPass.h"

#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanRenderPass : public RenderPass
	{
	public:
		VulkanRenderPass(const RenderPassProperties& aProperties);
		virtual ~VulkanRenderPass();

		void Destroy() override;

		void BeginRenderPass(const std::shared_ptr<CommandBuffer>& aCommandBuffer, const std::shared_ptr<Framebuffer>& aFramebuffer) const override;
		void EndRenderPass(const std::shared_ptr<CommandBuffer>& aCommandBuffer) const override;

		[[nodiscard]] const RenderPassProperties& GetProperties() const override { return mProperties; }

		[[nodiscard]] const VkRenderPass GetRenderPass() const { return mRenderPass; }
	private:
		RenderPassProperties mProperties;

		VkRenderPass mRenderPass;
		std::vector<VkClearValue> mClearValues;
	};
}