#include "ilargipch.h"

// Main headers
#include "VulkanSwapchain.h"
#include "VulkanGraphicsContext.h"
#include "Renderer/Renderer.h"

#include "VulkanCommandBuffer.h"

namespace Ilargi
{
	namespace Utils
	{
		static uint32_t GetAASamples(VkSampleCountFlags sampleCount)
		{
			if (sampleCount & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
			if (sampleCount & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
			if (sampleCount & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
			if (sampleCount & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
			if (sampleCount & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
			if (sampleCount & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

			return VK_SAMPLE_COUNT_1_BIT;
		}
	}

	VulkanSwapchain::VulkanSwapchain() : mSwapchain(VK_NULL_HANDLE), mCurrentFrame(0), mCurrentImageIndex(0)
	{
		auto device{ VulkanGraphicsContext::GetLogicalDevice() };

		QuerySwapchainSupport(VulkanGraphicsContext::GetPhysicalDevice());

		CreateSwapchain();

		CreateRenderPass(device);

		CreateFramebuffers();

		uint32_t imageCount{ Renderer::GetConfig().maxFrames };

		// Creating command buffers
		{
			mCommandBuffers.resize(imageCount);

			VkCommandBufferAllocateInfo allocInfo
			{
				VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, // sType
				nullptr,										// pNext
				VulkanGraphicsContext::GetCommandPool(),				// commandPool
				VK_COMMAND_BUFFER_LEVEL_PRIMARY,				// level
				static_cast<uint32_t>(mCommandBuffers.size())	// commandBufferCount
			};

			VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &allocInfo, mCommandBuffers.data()));
		}

		// Creating semaphores and fences
		{
			mImageAvailable.resize(imageCount);
			mRenderFinished.resize(imageCount);
			mFences.resize(imageCount);

			VkSemaphoreCreateInfo semaphoreInfo
			{
				VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,	// sType
				nullptr,									// pNext
				0											// flags
			};
			
			VkFenceCreateInfo fenceInfo
			{
				VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,	// sType
				nullptr,								// pNext
				VK_FENCE_CREATE_SIGNALED_BIT			// flags
			};
			
			for (uint32_t i { 0 }; i < imageCount; ++i)
			{
				VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &mImageAvailable[i]));

				VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &mRenderFinished[i]));

				VK_CHECK_RESULT(vkCreateFence(device, &fenceInfo, nullptr, &mFences[i]));
			}

			vkGetDeviceQueue(device, VulkanGraphicsContext::GetQueueIndices().presentFamily, 0, &mPresentQueue);
		}
	}
	
	VulkanSwapchain::~VulkanSwapchain()
	{
	}

	void VulkanSwapchain::StartFrame()
	{
		auto device{ VulkanGraphicsContext::GetLogicalDevice() };

		vkAcquireNextImageKHR(device, mSwapchain, UINT64_MAX, mImageAvailable[mCurrentFrame], VK_NULL_HANDLE, &mCurrentImageIndex);
	}

	void VulkanSwapchain::EndFrame()
	{
		auto device{ VulkanGraphicsContext::GetLogicalDevice() };

		VkPipelineStageFlags waitStages[] { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		
		VkSubmitInfo submitInfo
		{
			.sType {VK_STRUCTURE_TYPE_SUBMIT_INFO},
			.pNext {nullptr},
			.waitSemaphoreCount {1U},
			.pWaitSemaphores {&mImageAvailable[mCurrentFrame]},
			.pWaitDstStageMask {waitStages},
			.commandBufferCount {1U},
			.pCommandBuffers {&mCommandBuffers[mCurrentFrame]},
			.signalSemaphoreCount {1U},
			.pSignalSemaphores {&mRenderFinished[mCurrentFrame]}
		};

		VK_CHECK_RESULT(vkResetFences(device, 1, &mFences[mCurrentFrame]));
		VK_CHECK_RESULT(vkQueueSubmit(VulkanGraphicsContext::GetGraphicsQueue(), 1, &submitInfo, mFences[mCurrentFrame]));

		Present(device, mRenderFinished[mCurrentFrame]);
	}

	void VulkanSwapchain::Present(VkDevice aDevice, VkSemaphore aRenderFinish)
	{
		VkPresentInfoKHR presentInfo
		{
			.sType {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR},
			.pNext {nullptr},
			.waitSemaphoreCount {1U},
			.pWaitSemaphores {&aRenderFinish},
			.swapchainCount {1U},
			.pSwapchains {&mSwapchain},
			.pImageIndices {&mCurrentImageIndex},
			.pResults {nullptr}
		};

		VkResult result{ vkQueuePresentKHR(mPresentQueue, &presentInfo) };
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				RecreateSwapchain();
				return;
			}
		}

		VK_CHECK_RESULT(vkWaitForFences(aDevice, 1, &mFences[mCurrentFrame], VK_TRUE, UINT64_MAX));

		mCurrentFrame = (mCurrentFrame + 1) % Renderer::GetConfig().maxFrames;
		Renderer::SetNewFrame(mCurrentFrame);
	}

	void VulkanSwapchain::Destroy()
	{
		auto device{ VulkanGraphicsContext::GetLogicalDevice() };

		vkDeviceWaitIdle(device);

		for (uint32_t i { 0 }; i < mImageAvailable.size(); ++i)
		{
			vkDestroySemaphore(device, mImageAvailable[i], nullptr);
			vkDestroySemaphore(device, mRenderFinished[i], nullptr);
			vkDestroyFence(device, mFences[i], nullptr);
		}

		vkDestroyRenderPass(device, mRenderPass, nullptr);
		
		CleanUpSwapchain();
	}
	
	void VulkanSwapchain::RecreateSwapchain()
	{
		vkDeviceWaitIdle(VulkanGraphicsContext::GetLogicalDevice());
		CleanUpSwapchain();

		CreateSwapchain();
		CreateFramebuffers();

		vkDeviceWaitIdle(VulkanGraphicsContext::GetLogicalDevice());
	}

	void VulkanSwapchain::CreateSwapchain()
	{
		auto device{ VulkanGraphicsContext::GetLogicalDevice() };
		auto physicalDevice{ VulkanGraphicsContext::GetPhysicalDevice() };
		auto surface{ VulkanGraphicsContext::GetSurface() };
		
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

		RendererConfig config;
		VkSampleCountFlags counts{ physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts };

		config.maxAASamples = Utils::GetAASamples(counts);
		config.maxAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy;

		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

		QuerySwapchainSupport(VulkanGraphicsContext::GetPhysicalDevice());
		mExtent = capabilities.currentExtent;
		
		uint32_t imageCount{ capabilities.minImageCount + 1 };

		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
		{
			imageCount = capabilities.maxImageCount;
		}
		config.maxFrames = imageCount;
		
		Renderer::SetConfig(config);

		VkSwapchainCreateInfoKHR swapchainInfo {};
		swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainInfo.surface = VulkanGraphicsContext::GetSurface();

		swapchainInfo.minImageCount = imageCount;
		swapchainInfo.imageFormat = mSurfaceFormat.format;
		swapchainInfo.imageColorSpace = mSurfaceFormat.colorSpace;
		swapchainInfo.imageExtent = mExtent;
		swapchainInfo.imageArrayLayers = 1;
		swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices{ VulkanGraphicsContext::GetQueueIndices() };
		uint32_t queueFamilyIndices[] { indices.graphicsFamily, indices.presentFamily };

		if (indices.graphicsFamily != indices.presentFamily)
		{
			swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainInfo.queueFamilyIndexCount = 2;
			swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchainInfo.queueFamilyIndexCount = 0; // Optional
			swapchainInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		swapchainInfo.preTransform = capabilities.currentTransform;
		swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		swapchainInfo.presentMode = ChooseSwapPresentMode(physicalDevice, surface);
		swapchainInfo.clipped = VK_TRUE;

		swapchainInfo.oldSwapchain = nullptr;

		VK_CHECK_RESULT(vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &mSwapchain));

		mSwapchainImages.resize(imageCount);
		VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device, mSwapchain, &imageCount, mSwapchainImages.data()))

		// Create image views
		{
			mImageViews.resize(mSwapchainImages.size());

			for (uint32_t i { 0 }; i < mSwapchainImages.size(); ++i)
			{
				VkImageViewCreateInfo createInfo
				{
					VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,	// sType
					nullptr,									// pNext
					0,											// flags
					mSwapchainImages[i],						// image
					VK_IMAGE_VIEW_TYPE_2D,						// viewType
					mSurfaceFormat.format,						// format
					{											// components
						VK_COMPONENT_SWIZZLE_IDENTITY,				// r
						VK_COMPONENT_SWIZZLE_IDENTITY,				// g
						VK_COMPONENT_SWIZZLE_IDENTITY,				// b
						VK_COMPONENT_SWIZZLE_IDENTITY				// a
					},
					{											// subresourceRange
						VK_IMAGE_ASPECT_COLOR_BIT,					// aspectMask
						0,											// baseMipLevel
						1,											// levelCount
						0,											// baseArrayLayer
						1											// layerCount
					}
				};

				VK_CHECK_RESULT(vkCreateImageView(device, &createInfo, nullptr, &mImageViews[i]));
			}
		}

		// Depth image and image view
		{
			VkImageCreateInfo imageInfo
			{
				VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,			// sType
				nullptr,										// pNext
				0,												// flags
				VK_IMAGE_TYPE_2D,								// imageType
				VK_FORMAT_D32_SFLOAT,							// format
				{												// extent
					mExtent.width,									// width
					mExtent.height,									// height
					1,												// depth
				},
				1,												// mipLevels
				1,												// arrayLayers
				VK_SAMPLE_COUNT_1_BIT,							// samples
				VK_IMAGE_TILING_OPTIMAL,						// tiling
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,	// usage
				VK_SHARING_MODE_EXCLUSIVE,						// sharingMode
				0,												// queueFamilyIndexCount
				nullptr,										// pQueueFamilyIndices
				VK_IMAGE_LAYOUT_UNDEFINED						// initialLayout
			};

			VulkanAllocator::AllocateImage(mDepthImage, imageInfo, VMA_MEMORY_USAGE_GPU_ONLY, "Swapchain");

			VkImageViewCreateInfo createInfo
			{
				VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,	// sType
				nullptr,									// pNext
				0,											// flags
				mDepthImage.image,							// image
				VK_IMAGE_VIEW_TYPE_2D,						// viewType
				VK_FORMAT_D32_SFLOAT,						// format
				{											// components
					VK_COMPONENT_SWIZZLE_IDENTITY,				// r
					VK_COMPONENT_SWIZZLE_IDENTITY,				// g
					VK_COMPONENT_SWIZZLE_IDENTITY,				// b
					VK_COMPONENT_SWIZZLE_IDENTITY,				// a
				},
				{											// subresourceRange
					VK_IMAGE_ASPECT_DEPTH_BIT,					// aspectMask
					0,											// baseMipLevel
					1,											// levelCount
					0,											// baseArrayLayer
					1											// layerCount
				}
			};

			VK_CHECK_RESULT(vkCreateImageView(device, &createInfo, nullptr, &mDepthImageView));
		}
	}

	void VulkanSwapchain::CreateFramebuffers()
	{
		auto device{ VulkanGraphicsContext::GetLogicalDevice() };
		// Creating the framebuffers
		{
			mFramebuffers.resize(mImageViews.size());

			for (uint32_t i { 0 }; i < mImageViews.size(); ++i)
			{
				std::array<VkImageView, 2> attachments { mImageViews[i], mDepthImageView };

				VkFramebufferCreateInfo framebufferInfo
				{
					.sType {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO},
					.pNext {nullptr},
					.flags {0U},
					.renderPass {mRenderPass},
					.attachmentCount {static_cast<uint32_t>(attachments.size())},
					.pAttachments {attachments.data()},
					.width {mExtent.width},
					.height {mExtent.height},
					.layers {1U}
				};

				VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &mFramebuffers[i]));
			}
		}
	}

	void VulkanSwapchain::CleanUpSwapchain()
	{
		auto device{ VulkanGraphicsContext::GetLogicalDevice() };

		VulkanAllocator::DestroyImage(mDepthImage);
		for (uint32_t i { 0 }; i < mFramebuffers.size(); ++i)
		{
			vkDestroyFramebuffer(device, mFramebuffers[i], nullptr);
			vkDestroyImageView(device, mImageViews[i], nullptr);
		}
		vkDestroyImageView(device, mDepthImageView, nullptr);

		vkDestroySwapchainKHR(device, mSwapchain, nullptr);
	}

	void VulkanSwapchain::CreateRenderPass(VkDevice aDevice)
	{
		VkAttachmentDescription colorAttachment
		{
			.flags {0U},
			.format {mSurfaceFormat.format},
			.samples {VK_SAMPLE_COUNT_1_BIT},
			.loadOp {VK_ATTACHMENT_LOAD_OP_CLEAR},
			.storeOp {VK_ATTACHMENT_STORE_OP_STORE},
			.stencilLoadOp {VK_ATTACHMENT_LOAD_OP_DONT_CARE},
			.stencilStoreOp {VK_ATTACHMENT_STORE_OP_DONT_CARE},
			.initialLayout {VK_IMAGE_LAYOUT_UNDEFINED},
			.finalLayout {VK_IMAGE_LAYOUT_PRESENT_SRC_KHR}
		};

		VkAttachmentReference colorAttachmentRef
		{
			.attachment {0U},
			.layout {VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
		};

		VkAttachmentDescription depthAttachment
		{
			.flags {0U},
			.format {VK_FORMAT_D32_SFLOAT},
			.samples {VK_SAMPLE_COUNT_1_BIT},
			.loadOp {VK_ATTACHMENT_LOAD_OP_CLEAR},
			.storeOp {VK_ATTACHMENT_STORE_OP_DONT_CARE},
			.stencilLoadOp {VK_ATTACHMENT_LOAD_OP_DONT_CARE},
			.stencilStoreOp {VK_ATTACHMENT_STORE_OP_DONT_CARE},
			.initialLayout {VK_IMAGE_LAYOUT_UNDEFINED},
			.finalLayout {VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}
		};

		VkAttachmentReference depthAttachmentRef
		{
			.attachment {1U},
			.layout {VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}
		};

		VkSubpassDescription subpass
		{
			.flags {0U},
			.pipelineBindPoint {VK_PIPELINE_BIND_POINT_GRAPHICS},
			.inputAttachmentCount {0U},
			.pInputAttachments {nullptr},
			.colorAttachmentCount {1U},
			.pColorAttachments {&colorAttachmentRef},
			.pResolveAttachments {nullptr},
			.pDepthStencilAttachment {&depthAttachmentRef},
			.preserveAttachmentCount {0U},
			.pPreserveAttachments {nullptr}
		};

		VkSubpassDependency dependency
		{
			.srcSubpass {VK_SUBPASS_EXTERNAL},
			.dstSubpass {0U},
			.srcStageMask {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT},
			.dstStageMask {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT},
			.srcAccessMask {0U},
			.dstAccessMask {VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT},
			.dependencyFlags {0U}
		};

		std::array<VkAttachmentDescription, 2> attachments { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo
		{
			.sType {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO},
			.pNext {nullptr},
			.flags {0U},
			.attachmentCount {static_cast<uint32_t>(attachments.size())},
			.pAttachments {attachments.data()},
			.subpassCount {1U},
			.pSubpasses {&subpass},
			.dependencyCount {1U},
			.pDependencies {&dependency}
		};

		VK_CHECK_RESULT(vkCreateRenderPass(aDevice, &renderPassInfo, nullptr, &mRenderPass));
	}

	void VulkanSwapchain::QuerySwapchainSupport(VkPhysicalDevice aDevice)
	{
		auto surface{ VulkanGraphicsContext::GetSurface() };

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(aDevice, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			std::vector<VkSurfaceFormatKHR> availableFormats(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(aDevice, surface, &formatCount, availableFormats.data());

			mSurfaceFormat = availableFormats[0];
			for (VkSurfaceFormatKHR availableFormat : availableFormats)
			{
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					mSurfaceFormat = availableFormat;
					break;
				}
			}
		}
	}
	
	VkPresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(VkPhysicalDevice aDevice, VkSurfaceKHR aSurface) const
	{
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(aDevice, aSurface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			std::vector<VkPresentModeKHR> availablePresentModes(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(aDevice, aSurface, &presentModeCount, nullptr);
			for (const auto& availablePresentMode : availablePresentModes)
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					return availablePresentMode;
				}
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}
}