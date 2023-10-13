#pragma once

#include "Renderer/Framebuffer.h"
#include "Platform/Vulkan/VulkanAllocator.h"

#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferProperties& props);
		virtual ~VulkanFramebuffer();

		void Init(VkRenderPass renderPass);
		void Destroy();

		void Resize(std::shared_ptr<RenderPass> renderPass, uint32_t width, uint32_t height) override;

		const FramebufferProperties& GetProperties() const override { return properties; }
		const VkFramebuffer GetFramebuffer() const { return framebuffer; }

		uint32_t GetWidth() const override { return properties.width; }
		uint32_t GetHeight() const override { return properties.height; }

		void* GetID() const;

	private:
		FramebufferProperties properties;

		Image image;
		Image depthImage;
		VkImageView imageView;
		VkImageView depthImageView;
		VkFramebuffer framebuffer;

		VkSampler sampler;

		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorSet descriptorSet;
	};
}