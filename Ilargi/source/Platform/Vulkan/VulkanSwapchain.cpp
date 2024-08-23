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

			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = VulkanContext::GetCommandPool();
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());

			VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &allocInfo, mCommandBuffers.data()));
		}

		// Creating semaphores and fences
		{
			mImageAvailable.resize(imageCount);
			mRenderFinished.resize(imageCount);
			mFences.resize(imageCount);

			VkSemaphoreCreateInfo semaphoreInfo = {};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			
			VkFenceCreateInfo fenceInfo = {};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			
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

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &mImageAvailable[mCurrentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &mRenderFinished[mCurrentFrame];

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &mCommandBuffers[mCurrentFrame];

		VK_CHECK_RESULT(vkResetFences(device, 1, &mFences[mCurrentFrame]));
		VK_CHECK_RESULT(vkQueueSubmit(VulkanContext::GetGraphicsQueue(), 1, &submitInfo, mFences[mCurrentFrame]));

		Present(device, mRenderFinished[mCurrentFrame]);
	}

	void VulkanSwapchain::Present(VkDevice aDevice, VkSemaphore aRenderFinish)
	{
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &aRenderFinish;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &mSwapchain;
		presentInfo.pImageIndices = &mCurrentImageIndex;

		presentInfo.pResults = nullptr;

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
				VkImageViewCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				createInfo.image = mSwapchainImages[i];

				createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				createInfo.format = mSurfaceFormat.format;

				createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

				createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				createInfo.subresourceRange.baseMipLevel = 0;
				createInfo.subresourceRange.levelCount = 1;
				createInfo.subresourceRange.baseArrayLayer = 0;
				createInfo.subresourceRange.layerCount = 1;

				VK_CHECK_RESULT(vkCreateImageView(device, &createInfo, nullptr, &mImageViews[i]));
			}
		}

		// Depth image and image view
		{
			VkImageCreateInfo imageInfo = {};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = mExtent.width;
			imageInfo.extent.height = mExtent.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = VK_FORMAT_D32_SFLOAT;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.flags = 0;

			VulkanAllocator::AllocateImage(mDepthImage, imageInfo, VMA_MEMORY_USAGE_GPU_ONLY);

			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = mDepthImage.image;

			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = VK_FORMAT_D32_SFLOAT;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

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

				VkFramebufferCreateInfo framebufferInfo = {};
				framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.renderPass = mRenderPass;
				framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
				framebufferInfo.pAttachments = attachments.data();
				framebufferInfo.width = mExtent.width;
				framebufferInfo.height = mExtent.height;
				framebufferInfo.layers = 1;

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
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = mSurfaceFormat.format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = VK_FORMAT_D32_SFLOAT;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

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