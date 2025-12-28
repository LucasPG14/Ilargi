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

		[[nodiscard]] const VkRenderPass GetRenderPass() const { return mRenderPass; }

		[[nodiscard]] const VkCommandBuffer GetCurrentCommand() const { return mCommandBuffers[mCurrentFrame]; }

		[[nodiscard]] const VkFramebuffer GetFramebuffer() const { return mFramebuffers[mCurrentImageIndex]; }

		[[nodiscard]] const uint32_t GetWidth() const { return mExtent.width; }
		[[nodiscard]] const uint32_t GetHeight() const { return mExtent.height; }

	private:
		void Present(VkDevice aDevice, VkSemaphore aRenderFinish);

		void RecreateSwapchain();
		void CreateSwapchain();
		void CreateFramebuffers();
		void CleanUpSwapchain();

		void CreateRenderPass(VkDevice aDevice);

		void QuerySwapchainSupport(VkPhysicalDevice aDevice);
		VkPresentModeKHR ChooseSwapPresentMode(VkPhysicalDevice aDevice, VkSurfaceKHR aSurface) const;
	
	private:
		VkSwapchainKHR mSwapchain;

		VkExtent2D mExtent;
		VkSurfaceFormatKHR mSurfaceFormat;

		VkRenderPass mRenderPass;

		// Color attachments
		std::vector<VkImage> mSwapchainImages;
		std::vector<VkImageView> mImageViews;
		std::vector<VkFramebuffer> mFramebuffers;

		// Depth attachments
		Image mDepthImage;
		VkImageView mDepthImageView;

		std::vector<VkCommandBuffer> mCommandBuffers;

		// Synchronization objects
		std::vector<VkSemaphore> mImageAvailable;
		std::vector<VkSemaphore> mRenderFinished;
		std::vector<VkFence> mFences;

		uint32_t mCurrentFrame;
		uint32_t mCurrentImageIndex;
		VkQueue mPresentQueue;
	};
}