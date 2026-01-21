#pragma once

#include "Renderer/RenderPass.h"

#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanRenderPass : public RenderPass
	{
	public:
		/*
		* @brief Constructor.
		* @param aProperties The properties of the render pass.
		*/
		VulkanRenderPass(const RenderPassProperties& aProperties);
		
		/*
		* @brief Destructor.
		*/
		virtual ~VulkanRenderPass();

		/*
		* @copydoc VulkanRenderPass::Destroy()
		*/
		void Destroy() override;

		/*
		* @copydoc VulkanRenderPass::BeginRenderPass()
		*/
		void BeginRenderPass(const std::shared_ptr<CommandBuffer>& aCommandBuffer, const std::shared_ptr<Framebuffer>& aFramebuffer) const override;
		
		/*
		* @copydoc VulkanRenderPass::EndRenderPass()
		*/
		void EndRenderPass(const std::shared_ptr<CommandBuffer>& aCommandBuffer) const override;

		/*
		* @copydoc VulkanRenderPass::GetProperties()
		*/
		[[nodiscard]] const RenderPassProperties& GetProperties() const override { return mProperties; }

		/*
		* @brief Returns the instance of the vulkan render pass.
		* @return The vulkan render pass.
		*/
		[[nodiscard]] const VkRenderPass GetRenderPass() const { return mRenderPass; }

	private:
		std::vector<VkClearValue> mClearValues; // The values to clear the framebuffer.
		VkRenderPass mRenderPass; // Instance of the vulkan render pass.

		RenderPassProperties mProperties; // The render pass properties.
	};
}