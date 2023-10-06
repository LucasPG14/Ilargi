#pragma once

#include "Renderer/Swapchain.h"

#include <vulkan/vulkan.h>
#include <glm.hpp>

namespace Ilargi
{
	struct SwapchainSupportDetails;

	// TODO: This needs to be removed
	class VertexBuffer;
	class IndexBuffer;

	class Framebuffer;
	class RenderPass;
	class Pipeline;
	class CommandBuffer;

	class VulkanSwapchain : public Swapchain
	{
	public:
		VulkanSwapchain();
		virtual ~VulkanSwapchain();

		void Destroy() const;
		
		void StartFrame() override;
		void EndFrame() override;

		VkRenderPass GetRenderPass() { return renderPass; }

		VkCommandBuffer GetCurrentCommand() { return commandBuffers[currentFrame]; }

		VkFramebuffer GetFramebuffer() const { return framebuffers[currentImageIndex]; }

		uint32_t GetWidth() const { return extent.width; }
		uint32_t GetHeight() const { return extent.height; }

	private:
		void Present(VkDevice device, VkSemaphore renderFinish);

		void RecreateSwapchain();
		void CreateSwapchain();
		void CreateFramebuffers();
		void CleanUpSwapchain() const;

		void TransitionSwapchainImage();

		void CreateRenderPass(VkDevice device);

		void QuerySwapchainSupport(VkPhysicalDevice device);
		VkPresentModeKHR ChooseSwapPresentMode(VkPhysicalDevice device, VkSurfaceKHR surface) const;
	private:
		VkSwapchainKHR swapchain;

		VkExtent2D extent;
		VkSurfaceFormatKHR surfaceFormat;

		VkRenderPass renderPass;

		std::vector<VkImage> swapchainImages;
		std::vector<VkImageView> imageViews;
		std::vector<VkFramebuffer> framebuffers;

		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailable;
		std::vector<VkSemaphore> renderFinished;
		std::vector<VkFence> fences;

		uint32_t currentFrame;
		uint32_t currentImageIndex;
		VkQueue presentQueue;
	};
}