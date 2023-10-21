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
		VulkanFramebuffer(const FramebufferProperties& props);
		virtual ~VulkanFramebuffer();

		void Init(VkRenderPass renderPass);
		void Destroy() override;

		void Resize(const std::shared_ptr<RenderPass>& renderPass, uint32_t width, uint32_t height) override;

		const FramebufferProperties& GetProperties() const override { return properties; }
		const VkFramebuffer GetFramebuffer() const { return framebuffer; }

		const std::vector<ImageFormat>& GetColorSpecifications() const override { return colorSpecifications; }
		const ImageFormat GetDepthSpecification() const override { return depthSpecification; }

		const uint32_t GetWidth() const override { return properties.width; }
		const uint32_t GetHeight() const override { return properties.height; }

		void* GetID() const override;

	private:
		FramebufferProperties properties;

		// Color images
		std::vector<ImageFormat> colorSpecifications;
		std::vector<VulkanAttachment> colorAttachments;

		// Depth image
		ImageFormat depthSpecification;
		VulkanAttachment depthAttachment;

		VkFramebuffer framebuffer;

		VkSampler sampler;

		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorSet descriptorSet;
	};
}