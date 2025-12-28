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

		[[nodiscard]] static VkInstance GetInstance() { return sInstance; }

		[[nodiscard]] static VkSurfaceKHR GetSurface() { return sSurface; }
		[[nodiscard]] static VkPhysicalDevice GetPhysicalDevice() { return sPhysicalDevice; }
		[[nodiscard]] static VkDevice GetLogicalDevice() { return sLogicalDevice; }

		[[nodiscard]] static QueueFamilyIndices GetQueueIndices() { return sIndices; }
		[[nodiscard]] static SwapchainSupportDetails GetSwapchainSupport() { return sSwapchainSupport; }
		
		[[nodiscard]] static VkCommandPool GetCommandPool() { return sCommandPool; }
		[[nodiscard]] static VkDescriptorPool GetDescriptorPool() { return sDescriptorPool; }
		
		[[nodiscard]] static VkQueue GetGraphicsQueue() { return sGraphicsQueue; }

		static const VkCommandBuffer BeginSingleCommandBuffer();
		static void EndSingleCommandBuffer(const VkCommandBuffer commandBuffer);

	private:
		[[nodiscard]] const std::vector<const char*> GetRequiredExtensions() const;

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