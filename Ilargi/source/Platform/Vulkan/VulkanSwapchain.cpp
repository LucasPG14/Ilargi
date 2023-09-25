#include "ilargipch.h"

#include "VulkanSwapchain.h"
#include "VulkanContext.h"
#include "Renderer/Renderer.h"

namespace Ilargi
{
	VulkanSwapchain::VulkanSwapchain() : swapchain(VK_NULL_HANDLE)
	{
        auto device = VulkanContext::GetLogicalDevice();

        SwapChainSupportDetails swapchainSupport = QuerySwapchainSupport(VulkanContext::GetPhysicalDevice());

        surfaceFormat = ChooseSwapSurfaceFormat(swapchainSupport.formats);
        extent = swapchainSupport.capabilities.currentExtent;

        uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

        if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount)
        {
            imageCount = swapchainSupport.capabilities.maxImageCount;
            Renderer::SetMaxFrames(imageCount);
        }

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

        swapchainInfo.preTransform = swapchainSupport.capabilities.currentTransform;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        swapchainInfo.presentMode = ChooseSwapPresentMode(swapchainSupport.presentModes);
        swapchainInfo.clipped = VK_TRUE;

        swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

        VK_CHECK_RESULT(vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &swapchain) == VK_SUCCESS, "Unable to create the swapchain");

        swapchainImages.resize(imageCount);
        VK_CHECK_RESULT(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data()) == VK_SUCCESS, "Unable to get swapchain images")

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

        CreateRenderPass(device);

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

                VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) == VK_SUCCESS, "Unable to create the framebuffer")
            }
        }

        // Creating command buffers
        {
            commandBuffers.resize(imageCount);

            VkCommandBufferAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = VulkanContext::GetCommandPool();
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

            VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) == VK_SUCCESS, "Unable to allocate the command buffers")
        }
    }
	
	VulkanSwapchain::~VulkanSwapchain()
	{
	}

    void VulkanSwapchain::Destroy() const
    {
        auto device = VulkanContext::GetLogicalDevice();

        vkDestroyRenderPass(device, renderPass, nullptr);
        
        for (size_t i = 0; i < framebuffers.size(); ++i)
        {
            vkDestroyFramebuffer(device, framebuffers[i], nullptr);
            vkDestroyImageView(device, imageViews[i], nullptr);
        }

        vkDestroySwapchainKHR(device, swapchain, nullptr);
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

    SwapChainSupportDetails VulkanSwapchain::QuerySwapchainSupport(VkPhysicalDevice device) const
    {
        auto surface = VulkanContext::GetSurface();

        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }
    
    VkSurfaceFormatKHR VulkanSwapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const
    {
        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }
    
    VkPresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const
    {
        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }
    
    VkExtent2D VulkanSwapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const
    {
        return capabilities.currentExtent;
    }
}