#pragma once

#include "Renderer/Context.h"

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Ilargi
{
	struct QueueFamilyIndices
	{
		uint32_t graphicsFamily;
		uint32_t presentFamily;
	};

	struct SwapChainSupportDetails
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

		static VkSurfaceKHR GetSurface() { return surface; }
		static VkPhysicalDevice GetPhysicalDevice() { return physicalDevice; }
		static VkDevice GetLogicalDevice() { return logicalDevice; }

		static QueueFamilyIndices GetQueueIndices() { return indices; }
		
		static VkCommandPool GetCommandPool() { return commandPool; }

	private:
		std::vector<const char*> GetRequiredExtensions();

		bool IsDeviceSuitable(VkPhysicalDevice device) const;
		
		SwapChainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device) const;
		QueueFamilyIndices FindQueueFamilies();

	private:
		static VkInstance instance;
#ifdef ILG_DEBUG
		VkDebugUtilsMessengerEXT debugMessenger;
#endif
		static VkSurfaceKHR surface;
		static VkPhysicalDevice physicalDevice;
		static VkDevice logicalDevice;

		static QueueFamilyIndices indices;
		static VkCommandPool commandPool;
	};
}