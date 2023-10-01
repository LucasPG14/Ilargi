#include "ilargipch.h"

// Main headers
#include "VulkanSwapchain.h"
#include "VulkanContext.h"
#include "Renderer/Renderer.h"

// TODO: To remove
#include "VulkanCommandBuffer.h"

namespace Ilargi
{
	VulkanSwapchain::VulkanSwapchain() : swapchain(VK_NULL_HANDLE), currentFrame(0), currentImageIndex(0)
	{
		auto device = VulkanContext::GetLogicalDevice();

		QuerySwapchainSupport(VulkanContext::GetPhysicalDevice());

		CreateSwapchain();

		CreateRenderPass(device);

		CreateFramebuffers();

		uint32_t imageCount = Renderer::GetMaxFrames();

		// Creating command buffers
		{
			commandBuffers.resize(imageCount);

			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = VulkanContext::GetCommandPool();
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

			VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) == VK_SUCCESS,
				"Unable to allocate the command buffers");
		}

		// Creating semaphores and fences
		{
			imageAvailable.resize(imageCount);
			renderFinished.resize(imageCount);
			fences.resize(imageCount);

			VkSemaphoreCreateInfo semaphoreInfo = {};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			

			VkFenceCreateInfo fenceInfo = {};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			
			for (uint32_t i = 0; i < imageCount; ++i)
			{
				VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailable[i]) == VK_SUCCESS,
					"Unable to create image available semaphores");

				VK_CHECK_RESULT(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinished[i]) == VK_SUCCESS,
					"Unable to create render finished semaphores");

				VK_CHECK_RESULT(vkCreateFence(device, &fenceInfo, nullptr, &fences[i]) == VK_SUCCESS,
					"Unable to create fences");
			}

			vkGetDeviceQueue(device, VulkanContext::GetQueueIndices().presentFamily, 0, &presentQueue);
		}
	}
	
	VulkanSwapchain::~VulkanSwapchain()
	{
	}

	void VulkanSwapchain::StartFrame()
	{
		auto device = VulkanContext::GetLogicalDevice();

		vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailable[currentFrame], VK_NULL_HANDLE, &currentImageIndex);
	}

	void VulkanSwapchain::EndFrame()
	{
		auto device = VulkanContext::GetLogicalDevice();

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &imageAvailable[currentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &renderFinished[currentFrame];

		auto cmdBuffersSubmit = Renderer::GetSubmittedCommands();
		std::vector<VkCommandBuffer> vkCommandBuffers;
		for (int i = 0; i < cmdBuffersSubmit.size(); ++i)
		{
			vkCommandBuffers.push_back(std::static_pointer_cast<VulkanCommandBuffer>(cmdBuffersSubmit[i])->GetCurrentCommand(currentFrame));
		}

		submitInfo.commandBufferCount = static_cast<uint32_t>(vkCommandBuffers.size());
		submitInfo.pCommandBuffers = vkCommandBuffers.data();

		vkResetFences(device, 1, &fences[currentFrame]);
		VK_CHECK_RESULT(vkQueueSubmit(VulkanContext::GetGraphicsQueue(), 1, &submitInfo, fences[currentFrame]) == VK_SUCCESS,
			"Failed to submit draw command buffer");

		Present(device, renderFinished[currentFrame], fences[currentFrame]);
	}

	void VulkanSwapchain::Present(VkDevice device, VkSemaphore renderFinish, VkFence fence)
	{
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinish;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain;
		presentInfo.pImageIndices = &currentImageIndex;

		presentInfo.pResults = nullptr;

		VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				RecreateSwapchain();
				return;
			}
		}

		currentFrame = (currentFrame + 1) % Renderer::GetMaxFrames();

		vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);

		Renderer::SetNewFrame(currentFrame);
	}

	void VulkanSwapchain::Destroy() const
	{
		auto device = VulkanContext::GetLogicalDevice();

		vkDeviceWaitIdle(device);

		for (size_t i = 0; i < imageAvailable.size(); ++i)
		{
			vkDestroySemaphore(device, imageAvailable[i], nullptr);
			vkDestroySemaphore(device, renderFinished[i], nullptr);
			vkDestroyFence(device, fences[i], nullptr);
		}

		vkDestroyRenderPass(device, renderPass, nullptr);
		
		CleanUpSwapchain();
		vkDestroySwapchainKHR(device, swapchain, nullptr);
	}
	
	void VulkanSwapchain::RecreateSwapchain()
	{
		vkDeviceWaitIdle(VulkanContext::GetLogicalDevice());
		CleanUpSwapchain();

		Renderer::ResetCurrentFrame();

		CreateSwapchain();
		CreateFramebuffers();
	}

	void VulkanSwapchain::CreateSwapchain()
	{
		auto device = VulkanContext::GetLogicalDevice();
		auto physicalDevice = VulkanContext::GetPhysicalDevice();
		auto surface = VulkanContext::GetSurface();
		
		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

		QuerySwapchainSupport(VulkanContext::GetPhysicalDevice());
		extent = capabilities.currentExtent;

		uint32_t imageCount = capabilities.minImageCount + 1;

		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
		{
			imageCount = capabilities.maxImageCount;
		}
		Renderer::SetMaxFrames(imageCount);

		VkSwapchainCreateInfoKHR swapchainInfo = {};
		swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainInfo.surface = VulkanContext::GetSurface();

		swapchainInfo.minImageCount = imageCount;
		swapchainInfo.imageFormat = surfaceFormat.format;
		swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapchainInfo.imageExtent = extent;
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

		swapchainInfo.oldSwapchain = swapchain;

		VK_CHECK_RESULT(vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &swapchain) == VK_SUCCESS, 
			"Unable to create the swapchain");

		swapchainImages.resize(imageCount);
		VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data()) == VK_SUCCESS, 
			"Unable to get swapchain images")

		// Create image views
		{
			imageViews.resize(swapchainImages.size());

			for (size_t i = 0; i < swapchainImages.size(); ++i)
			{
				VkImageViewCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				createInfo.image = swapchainImages[i];

				createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				createInfo.format = surfaceFormat.format;

				createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

				createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				createInfo.subresourceRange.baseMipLevel = 0;
				createInfo.subresourceRange.levelCount = 1;
				createInfo.subresourceRange.baseArrayLayer = 0;
				createInfo.subresourceRange.layerCount = 1;

				VK_CHECK_RESULT(vkCreateImageView(device, &createInfo, nullptr, &imageViews[i]) == VK_SUCCESS, "Unable to create image views");
			}
		}
	}

	void VulkanSwapchain::CreateFramebuffers()
	{
		auto device = VulkanContext::GetLogicalDevice();
		// Creating the framebuffers
		{
			framebuffers.resize(imageViews.size());

			for (size_t i = 0; i < imageViews.size(); i++)
			{
				std::array<VkImageView, 1> attachments = { imageViews[i] };

				VkFramebufferCreateInfo framebufferInfo = {};
				framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.renderPass = renderPass;
				framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
				framebufferInfo.pAttachments = attachments.data();
				framebufferInfo.width = extent.width;
				framebufferInfo.height = extent.height;
				framebufferInfo.layers = 1;

				VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) == VK_SUCCESS, "Unable to create the framebuffer");
			}
		}
	}

	void VulkanSwapchain::CleanUpSwapchain() const
	{
		auto device = VulkanContext::GetLogicalDevice();
		for (size_t i = 0; i < framebuffers.size(); ++i)
		{
			vkDestroyFramebuffer(device, framebuffers[i], nullptr);
			vkDestroyImageView(device, imageViews[i], nullptr);
		}
	}

	void VulkanSwapchain::TransitionSwapchainImage()
	{
		VkCommandBuffer commandBuffer = VulkanContext::BeginSingleCommandBuffer();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = swapchainImages[currentImageIndex];
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		VulkanContext::EndSingleCommandBuffer(commandBuffer);
	}

	void VulkanSwapchain::CreateRenderPass(VkDevice device)
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = surfaceFormat.format;
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

		// TODO: Depth attachment
		//VkAttachmentDescription depthAttachment{};
		//depthAttachment.format = VK_FORMAT_D32_SFLOAT;
		//depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		//depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		//depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		//depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		//depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		//depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		//depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		//VkAttachmentReference depthAttachmentRef{};
		//depthAttachmentRef.attachment = 1;
		//depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		//subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 1> attachments = { colorAttachment };
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) == VK_SUCCESS, "Unable to create the render pass");
	}

	void VulkanSwapchain::QuerySwapchainSupport(VkPhysicalDevice device)
	{
		auto surface = VulkanContext::GetSurface();

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			std::vector<VkSurfaceFormatKHR> availableFormats(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, availableFormats.data());

			surfaceFormat = availableFormats[0];
			for (VkSurfaceFormatKHR availableFormat : availableFormats)
			{
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					surfaceFormat = availableFormat;
					break;
				}
			}
		}
	}
	
	VkPresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(VkPhysicalDevice device, VkSurfaceKHR surface) const
	{
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			std::vector<VkPresentModeKHR> availablePresentModes(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
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