#pragma once

#include "Renderer/Swapchain.h"

#include <vulkan/vulkan.h>

namespace Ilargi
{
	struct SwapChainSupportDetails;

	class VulkanSwapchain : public Swapchain
	{
	public:
		VulkanSwapchain();
		virtual ~VulkanSwapchain();

		void Destroy() const;

	private:
		void CreateRenderPass(VkDevice device);

		SwapChainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device) const;
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;

		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
	private:
		VkSwapchainKHR swapchain;

		VkExtent2D extent;
		VkSurfaceFormatKHR surfaceFormat;

		VkRenderPass renderPass;

		std::vector<VkImage> swapchainImages;
		std::vector<VkImageView> imageViews;
		std::vector<VkFramebuffer> framebuffers;

		std::vector<VkCommandBuffer> commandBuffers;
	};
}