#pragma once

#include "Renderer/Framebuffer.h"

#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferProperties& props);
		virtual ~VulkanFramebuffer();

		void Init(VkRenderPass renderPass);

		const FramebufferProperties& GetProperties() const override { return properties; }

	private:
		FramebufferProperties properties;

		VkImage image;
		VkImageView imageView;
		VkFramebuffer framebuffer;
	};
}