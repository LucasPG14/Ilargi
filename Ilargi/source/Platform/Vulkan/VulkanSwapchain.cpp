#include "ilargipch.h"

// Main headers
#include "VulkanSwapchain.h"
#include "VulkanContext.h"
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
		auto device = VulkanContext::GetLogicalDevice();

		QuerySwapchainSupport(VulkanContext::GetPhysicalDevice());

		CreateSwapchain();

		CreateRenderPass(device);

		CreateFramebuffers();

		uint32_t imageCount = Renderer::GetConfig().maxFrames;

		// Creating command buffers
		{
			mCommandBuffers.resize(imageCount);

			VkCommandBufferAllocateInfo allocInfo
			{
				VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, // sType
				nullptr,										// pNext
				VulkanContext::GetCommandPool(),				// commandPool
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
			
			for (uint32_t i = 0; i < imageCount; ++i)
			{
				VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &mImageAvailable[i]));

				VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &mRenderFinished[i]));

				VK_CHECK_RESULT(vkCreateFence(device, &fenceInfo, nullptr, &mFences[i]));
			}

			vkGetDeviceQueue(device, VulkanContext::GetQueueIndices().presentFamily, 0, &mPresentQueue);
		}
	}
	
	VulkanSwapchain::~VulkanSwapchain()
	{
	}

	void VulkanSwapchain::StartFrame()
	{
		auto device = VulkanContext::GetLogicalDevice();

		vkAcquireNextImageKHR(device, mSwapchain, UINT64_MAX, mImageAvailable[mCurrentFrame], VK_NULL_HANDLE, &mCurrentImageIndex);
	}

	void VulkanSwapchain::EndFrame()
	{
		auto device = VulkanContext::GetLogicalDevice();

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		
		VkSubmitInfo submitInfo
		{
			VK_STRUCTURE_TYPE_SUBMIT_INFO,		// sType
			nullptr,							// pNext
			1,									// waitSemaphoreCount
			&mImageAvailable[mCurrentFrame],	// pWaitSemaphores
			waitStages,							// pWaitDstStageMask
			1,									// commandBufferCount
			&mCommandBuffers[mCurrentFrame],	// pCommandBuffers
			1,									// signalSemaphoreCount
			&mRenderFinished[mCurrentFrame]		// pSignalSemaphores
		};

		VK_CHECK_RESULT(vkResetFences(device, 1, &mFences[mCurrentFrame]));
		VK_CHECK_RESULT(vkQueueSubmit(VulkanContext::GetGraphicsQueue(), 1, &submitInfo, mFences[mCurrentFrame]));

		Present(device, mRenderFinished[mCurrentFrame]);
	}

	void VulkanSwapchain::Present(VkDevice aDevice, VkSemaphore aRenderFinish)
	{
		VkPresentInfoKHR presentInfo
		{
			VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,		// sType
			nullptr,								// pNext
			1,										// waitSemaphoreCount
			&aRenderFinish,							// pWaitSemaphores
			1,										// swapchainCount
			&mSwapchain,							// pSwapchains
			&mCurrentImageIndex,					// pImageIndices
			nullptr									// pResults
		};

		VkResult result = vkQueuePresentKHR(mPresentQueue, &presentInfo);
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
		auto device = VulkanContext::GetLogicalDevice();

		vkDeviceWaitIdle(device);

		for (size_t i = 0; i < mImageAvailable.size(); ++i)
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
		vkDeviceWaitIdle(VulkanContext::GetLogicalDevice());
		CleanUpSwapchain();

		CreateSwapchain();
		CreateFramebuffers();

		vkDeviceWaitIdle(VulkanContext::GetLogicalDevice());
	}

	void VulkanSwapchain::CreateSwapchain()
	{
		auto device = VulkanContext::GetLogicalDevice();
		auto physicalDevice = VulkanContext::GetPhysicalDevice();
		auto surface = VulkanContext::GetSurface();
		
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

		RendererConfig config;
		VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;

		config.maxAASamples = Utils::GetAASamples(counts);
		config.maxAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy;

		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

		QuerySwapchainSupport(VulkanContext::GetPhysicalDevice());
		mExtent = capabilities.currentExtent;
		
		uint32_t imageCount = capabilities.minImageCount + 1;

		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
		{
			imageCount = capabilities.maxImageCount;
		}
		config.maxFrames = imageCount;
		
		Renderer::SetConfig(config);

		VkSwapchainCreateInfoKHR swapchainInfo = {};
		swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainInfo.surface = VulkanContext::GetSurface();

		swapchainInfo.minImageCount = imageCount;
		swapchainInfo.imageFormat = mSurfaceFormat.format;
		swapchainInfo.imageColorSpace = mSurfaceFormat.colorSpace;
		swapchainInfo.imageExtent = mExtent;
		swapchainInfo.imageArrayLayers = 1;
		swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = VulkanContext::GetQueueIndices();
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

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

			for (size_t i = 0; i < mSwapchainImages.size(); ++i)
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

			VulkanAllocator::AllocateImage(mDepthImage, imageInfo, VMA_MEMORY_USAGE_GPU_ONLY);

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
		auto device = VulkanContext::GetLogicalDevice();
		// Creating the framebuffers
		{
			mFramebuffers.resize(mImageViews.size());

			for (size_t i = 0; i < mImageViews.size(); i++)
			{
				std::array<VkImageView, 2> attachments = { mImageViews[i], mDepthImageView };

				VkFramebufferCreateInfo framebufferInfo
				{
					VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,	// sType
					nullptr,									// pNext
					0,											// flags
					mRenderPass,								// renderPass
					static_cast<uint32_t>(attachments.size()),	// attachmentCount
					attachments.data(),							// pAttachments
					mExtent.width,								// width
					mExtent.height,								// height
					1											// layers
				};

				VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &mFramebuffers[i]));
			}
		}
	}

	void VulkanSwapchain::CleanUpSwapchain()
	{
		auto device = VulkanContext::GetLogicalDevice();

		VulkanAllocator::DestroyImage(mDepthImage);
		for (size_t i = 0; i < mFramebuffers.size(); ++i)
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
			0,									// flags
			mSurfaceFormat.format,				// format
			VK_SAMPLE_COUNT_1_BIT,				// samples
			VK_ATTACHMENT_LOAD_OP_CLEAR,		// loadOp
			VK_ATTACHMENT_STORE_OP_STORE,		// storeOp
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,	// stencilLoadOp
			VK_ATTACHMENT_STORE_OP_DONT_CARE,	// stencilStoreOp
			VK_IMAGE_LAYOUT_UNDEFINED,			// initialLayout
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR		// finalLayout
		};

		VkAttachmentReference colorAttachmentRef
		{
			0,											// attachment
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL	// layout
		};

		VkAttachmentDescription depthAttachment
		{
			0,													// flags
			VK_FORMAT_D32_SFLOAT,								// format
			VK_SAMPLE_COUNT_1_BIT,								// samples
			VK_ATTACHMENT_LOAD_OP_CLEAR,						// loadOp
			VK_ATTACHMENT_STORE_OP_DONT_CARE,					// storeOp
			VK_ATTACHMENT_LOAD_OP_DONT_CARE,					// stencilLoadOp
			VK_ATTACHMENT_STORE_OP_DONT_CARE,					// stencilStoreOp
			VK_IMAGE_LAYOUT_UNDEFINED,							// initialLayout
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL	// finalLayout
		};

		VkAttachmentReference depthAttachmentRef
		{
			1,													// attachment
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL	// layout
		};

		VkSubpassDescription subpass
		{
			0,									// flags
			VK_PIPELINE_BIND_POINT_GRAPHICS,	// pipelineBindPoint
			0,									// inputAttachmentCount
			nullptr,							// pInputAttachments
			1,									// colorAttachmentCount
			&colorAttachmentRef,				// pColorAttachments
			nullptr,							// pResolveAttachments
			&depthAttachmentRef,				// pDepthStencilAttachment
			0,									// preserveAttachmentCount
			nullptr								// pPreserveAttachments
		};

		VkSubpassDependency dependency
		{
			VK_SUBPASS_EXTERNAL,																			// srcSubpass
			0,																								// dstSubpass
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,		// srcStageMask
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,		// dstStageMask
			0,																								// srcAccessMask
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,			// dstAccessMask
			0																								// dependencyFlags
		};

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,	// sType
			nullptr,									// pNext
			0,											// flags
			static_cast<uint32_t>(attachments.size()),	// attachmentCount
			attachments.data(),							// pAttachments
			1,											// subpassCount
			&subpass,									// pSubpasses
			1,											// dependencyCount
			&dependency									// pDependencies
		};

		VK_CHECK_RESULT(vkCreateRenderPass(aDevice, &renderPassInfo, nullptr, &mRenderPass));
	}

	void VulkanSwapchain::QuerySwapchainSupport(VkPhysicalDevice aDevice)
	{
		auto surface = VulkanContext::GetSurface();

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