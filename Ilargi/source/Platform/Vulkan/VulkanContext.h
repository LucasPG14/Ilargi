#pragma once

#include "Renderer/Context.h"
#include "VulkanUtils.h"
#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Ilargi
{
	struct QueueFamilyIndices
	{
		uint32_t graphicsFamily;
		uint32_t presentFamily;
	};

	struct SwapchainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities = {};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext(GLFWwindow* win, std::string_view appName);
		virtual ~VulkanContext();

		void Destroy() const override;

		static VkInstance GetInstance() { return instance; }

		static VkSurfaceKHR GetSurface() { return surface; }
		static VkPhysicalDevice GetPhysicalDevice() { return physicalDevice; }
		static VkDevice GetLogicalDevice() { return logicalDevice; }

		static QueueFamilyIndices GetQueueIndices() { return indices; }
		static SwapchainSupportDetails GetSwapchainSupport() { return swapchainSupport; }
		
		static VkCommandPool GetCommandPool() { return commandPool; }
		static VkDescriptorPool GetDescriptorPool() { return descriptorPool; }
		
		static VkQueue GetGraphicsQueue() { return graphicsQueue; }

		static const VkCommandBuffer BeginSingleCommandBuffer();
		static void EndSingleCommandBuffer(const VkCommandBuffer commandBuffer);

	private:
		const std::vector<const char*> GetRequiredExtensions() const;

		bool IsDeviceSuitable(VkPhysicalDevice device) const;
		
		bool CanCreateSwapchain(VkPhysicalDevice device) const;
		const QueueFamilyIndices FindQueueFamilies() const;

	private:
		static VkInstance instance;
#ifdef ILG_DEBUG
		VkDebugUtilsMessengerEXT debugMessenger;
#endif
		static VkSurfaceKHR surface;
		static VkPhysicalDevice physicalDevice;
		static VkDevice logicalDevice;

		static QueueFamilyIndices indices;
		static SwapchainSupportDetails swapchainSupport;

		static VkCommandPool commandPool;

		static VkQueue graphicsQueue;

		static VkDescriptorPool descriptorPool;
	};
}