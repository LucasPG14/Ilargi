#pragma once

#include "Renderer/Swapchain.h"

#include "VulkanAllocator.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanSwapchain : public Swapchain
	{
	public:
		/*
		* @brief Constructor.
		*/
		VulkanSwapchain();

		/*
		* @brief Destructor.
		*/
		virtual ~VulkanSwapchain();

		/*
		* @copydoc Swapchain::Destroy()
		*/
		void Destroy() override;
		
		/*
		* @copydoc Swapchain::StartFrame()
		*/
		void StartFrame() override;

		/*
		* @copydoc Swapchain::EndFrame()
		*/
		void EndFrame() override;

		/*
		* @brief Returns the render pass used by the swapchain.
		* @return The swapchain render pass.
		*/
		[[nodiscard]] const VkRenderPass GetRenderPass() const { return mRenderPass; }

		/*
		* @brief Returns the current command buffer of the swapchain.
		* @return The current swapchain command buffer.
		*/
		[[nodiscard]] const VkCommandBuffer GetCurrentCommand() const { return mCommandBuffers[mCurrentFrame]; }

		/*
		* @brief Returns the current framebuffer of the swapchain.
		* @return The current swapchain framebuffer.
		*/
		[[nodiscard]] const VkFramebuffer GetFramebuffer() const { return mFramebuffers[mCurrentImageIndex]; }

		/*
		* @brief Returns the width of the swapchain.
		* @return The swapchain width.
		*/
		[[nodiscard]] const uint32_t GetWidth() const { return mExtent.width; }

		/*
		* @brief Returns the height of the swapchain.
		* @return The swapchain height.
		*/
		[[nodiscard]] const uint32_t GetHeight() const { return mExtent.height; }

	private:
		/*
		* @brief Presents the framebuffer on the screen.
		* @param aDevice The vulkan device.
		* @param aRenderFinish The vulkan semaphore indicating the render has finished.
		*/
		void Present(VkDevice aDevice, VkSemaphore aRenderFinish);

		/*
		* @brief Destroys and creates the swapchain again.
		*/
		void RecreateSwapchain();

		/*
		* @brief Creates the swapchain.
		*/
		void CreateSwapchain();

		/*
		* @brief Creates the framebuffers for the swapchain.
		*/
		void CreateFramebuffers();

		/*
		* @brief Cleans all swapchain data.
		*/
		void CleanUpSwapchain();

		/*
		* @brief Creates the render pass for the swapchain.
		*/
		void CreateRenderPass(VkDevice aDevice);

		/*
		* @brief Gets the swapchain surface format supported.
		* @param aDevice Instance of the vulkan physical device.
		*/
		void QuerySwapchainSupport(VkPhysicalDevice aPhysicalDevice);
		
		/*
		* @brief Returns the present mode for the swapchain.
		* @param aDevice Instance of the vulkan physical device.
		* @param aSurface Instance of the vulkan surface.
		* @return The swapchain present mode.
		*/
		VkPresentModeKHR ChooseSwapPresentMode(VkPhysicalDevice aPhysicalDevice, VkSurfaceKHR aSurface) const;
	
	private:
		// Color attachments
		std::vector<VkImage> mSwapchainImages; // Container with the swapchain images.
		std::vector<VkImageView> mImageViews; // Container with the swapchain image views.
		std::vector<VkFramebuffer> mFramebuffers; // Container with the swapchain framebuffers.

		std::vector<VkCommandBuffer> mCommandBuffers; // Container with the command buffers.

		// Synchronization objects
		std::vector<VkSemaphore> mImageAvailable; // Container with the render available semaphores.
		std::vector<VkSemaphore> mRenderFinished; // Container with the render finished semaphores.
		std::vector<VkFence> mFences; // Container with the fences,

		// Depth attachments
		Image mDepthImage; // The depth image of the swapchain.
		VkImageView mDepthImageView; // The depth image view of the swapchain.

		VkSwapchainKHR mSwapchain; // Instance of the Vulkan swapchain.
		VkRenderPass mRenderPass; // Instance of the swapchain render pass.
		VkQueue mPresentQueue; // Instance of the Vulkan swapchain present queue.

		VkExtent2D mExtent; // The width and height of the swapchain.
		VkSurfaceFormatKHR mSurfaceFormat; // The surface format of the swapchain.

		uint32_t mCurrentFrame; // Indicates the current frame. Used to select the current command buffer.
		uint32_t mCurrentImageIndex; // Indicates the current image. Used to select the current framebuffer.
	};
}