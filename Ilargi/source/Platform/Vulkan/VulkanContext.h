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
		VkSurfaceCapabilitiesKHR capabilities {};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext(GLFWwindow* aWindow, std::string_view aAppName);
		virtual ~VulkanContext();

		void Destroy() const override;

		static VkInstance GetInstance() { return sInstance; }

		static VkSurfaceKHR GetSurface() { return sSurface; }
		static VkPhysicalDevice GetPhysicalDevice() { return sPhysicalDevice; }
		static VkDevice GetLogicalDevice() { return sLogicalDevice; }

		static QueueFamilyIndices GetQueueIndices() { return sIndices; }
		static SwapchainSupportDetails GetSwapchainSupport() { return sSwapchainSupport; }
		
		static VkCommandPool GetCommandPool() { return sCommandPool; }
		static VkDescriptorPool GetDescriptorPool() { return sDescriptorPool; }
		
		static VkQueue GetGraphicsQueue() { return sGraphicsQueue; }

		static const VkCommandBuffer BeginSingleCommandBuffer();
		static void EndSingleCommandBuffer(const VkCommandBuffer commandBuffer);

	private:
		const std::vector<const char*> GetRequiredExtensions() const;

		bool IsDeviceSuitable(VkPhysicalDevice aDevice) const;
		
		bool CanCreateSwapchain(VkPhysicalDevice aDevice) const;
		const QueueFamilyIndices FindQueueFamilies() const;

	private:
		static VkInstance sInstance;
#ifdef ILG_DEBUG
		VkDebugUtilsMessengerEXT mDebugMessenger;
#endif
		static VkSurfaceKHR sSurface;
		static VkPhysicalDevice sPhysicalDevice;
		static VkDevice sLogicalDevice;

		static QueueFamilyIndices sIndices;
		static SwapchainSupportDetails sSwapchainSupport;

		static VkCommandPool sCommandPool;

		static VkQueue sGraphicsQueue;

		static VkDescriptorPool sDescriptorPool;
	};
}