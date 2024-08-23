#pragma once

#include "Renderer/Framebuffer.h"
#include "Platform/Vulkan/VulkanAllocator.h"

#include <vulkan/vulkan.h>

namespace Ilargi
{
	struct VulkanAttachment
	{
		Image image;
		VkImageView imageView;
	};

	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferProperties& aProperties);
		virtual ~VulkanFramebuffer();

		void Init(VkRenderPass aRenderPass);
		void Destroy() override;

		void Resize(const std::shared_ptr<RenderPass>& aRenderPass, uint32_t aWidth, uint32_t aHeight) override;

		const FramebufferProperties& GetProperties() const override { return mProperties; }
		const VkFramebuffer GetFramebuffer() const { return mFramebuffer; }

		const std::vector<ImageFormat>& GetColorSpecifications() const override { return mColorSpecifications; }
		const ImageFormat GetDepthSpecification() const override { return mDepthSpecification; }

		const uint32_t GetWidth() const override { return mProperties.width; }
		const uint32_t GetHeight() const override { return mProperties.height; }

		void* GetID() const override;

	private:
		FramebufferProperties mProperties;

		// Color images
		std::vector<ImageFormat> mColorSpecifications;
		std::vector<VulkanAttachment> mColorAttachments;

		// Depth image
		ImageFormat mDepthSpecification;
		VulkanAttachment mDepthAttachment;

		VkFramebuffer mFramebuffer;

		VkSampler mSampler;

		VkDescriptorSetLayout mDescriptorSetLayout;
		VkDescriptorSet mDescriptorSet;
	};
}