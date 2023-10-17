#pragma once

#include "Renderer/Swapchain.h"

#include "VulkanAllocator.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanSwapchain : public Swapchain
	{
	public:
		VulkanSwapchain();
		virtual ~VulkanSwapchain();

		void Destroy();
		
		void StartFrame() override;
		void EndFrame() override;

		const VkRenderPass GetRenderPass() const { return renderPass; }

		const VkCommandBuffer GetCurrentCommand() const { return commandBuffers[currentFrame]; }

		const VkFramebuffer GetFramebuffer() const { return framebuffers[currentImageIndex]; }

		const uint32_t GetWidth() const { return extent.width; }
		const uint32_t GetHeight() const { return extent.height; }

	private:
		void Present(VkDevice device, VkSemaphore renderFinish);

		void RecreateSwapchain();
		void CreateSwapchain();
		void CreateFramebuffers();
		void CleanUpSwapchain();

		void CreateRenderPass(VkDevice device);

		void QuerySwapchainSupport(VkPhysicalDevice device);
		VkPresentModeKHR ChooseSwapPresentMode(VkPhysicalDevice device, VkSurfaceKHR surface) const;
	private:
		VkSwapchainKHR swapchain;

		VkExtent2D extent;
		VkSurfaceFormatKHR surfaceFormat;

		VkRenderPass renderPass;

		// Color attachments
		std::vector<VkImage> swapchainImages;
		std::vector<VkImageView> imageViews;
		std::vector<VkFramebuffer> framebuffers;

		// Depth attachments
		Image depthImage;
		VkImageView depthImageView;

		std::vector<VkCommandBuffer> commandBuffers;

		// Synchronization objects
		std::vector<VkSemaphore> imageAvailable;
		std::vector<VkSemaphore> renderFinished;
		std::vector<VkFence> fences;

		uint32_t currentFrame;
		uint32_t currentImageIndex;
		VkQueue presentQueue;
	};
}