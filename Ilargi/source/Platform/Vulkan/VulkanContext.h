#pragma once

#include "Renderer/Context.h"
#include "VulkanUtils.h"
#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Ilargi
{
	struct QueueFamilyIndices
	{
		uint32_t graphicsFamily{0U};
		uint32_t presentFamily{0U};
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
		/*
		* @brief Constructor.
		* @param aWindow The current GLFW window pointer.
		* @param aAppName The application name.
		*/
		VulkanContext(GLFWwindow* aWindow, std::string_view aAppName);

		/*
		* @brief Destructor.
		*/
		virtual ~VulkanContext();

		/*
		* @copydoc GraphicsContext::Destroy()
		*/
		void Destroy() const override;

		/*
		* @brief Returns the vulkan instance.
		* @return The vulkan instance.
		*/
		[[nodiscard]] static VkInstance GetInstance() { return sInstance; }

		/*
		* @brief Returns the vulkan surface.
		* @return The vulkan surface.
		*/
		[[nodiscard]] static VkSurfaceKHR GetSurface() { return sSurface; }

		/*
		* @brief Returns the vulkan physical device.
		* @return The vulkan physical device.
		*/
		[[nodiscard]] static VkPhysicalDevice GetPhysicalDevice() { return sPhysicalDevice; }

		/*
		* @brief Returns the vulkan logical device.
		* @return The vulkan logical device.
		*/
		[[nodiscard]] static VkDevice GetLogicalDevice() { return sLogicalDevice; }

		/*
		* @brief Returns the vulkan queue indices.
		* @return The vulkan queue indices.
		*/
		[[nodiscard]] static QueueFamilyIndices GetQueueIndices() { return sIndices; }

		/*
		* @brief Returns the vulkan swapchain support information.
		* @return The vulkan swapchain support information.
		*/
		[[nodiscard]] static SwapchainSupportDetails GetSwapchainSupport() { return sSwapchainSupport; }
		
		/*
		* @brief Returns the vulkan command pool.
		* @return The vulkan command pool.
		*/
		[[nodiscard]] static VkCommandPool GetCommandPool() { return sCommandPool; }

		/*
		* @brief Returns the vulkan descriptor pool.
		* @return The vulkan descriptor pool.
		*/
		[[nodiscard]] static VkDescriptorPool GetDescriptorPool() { return sDescriptorPool; }

		/*
		* @brief Returns the vulkan pipeline layout.
		* @return The vulkan pipeline layout.
		*/
		[[nodiscard]] static VkPipelineLayout GetPipelineLayout() { return sPipelineLayout; }

		/*
		* @brief Returns the vulkan pipeline layout.
		* @return The vulkan pipeline layout.
		*/
		[[nodiscard]] static std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayouts() { return sDescriptorSetLayouts; }
		
		/*
		* @brief Returns the vulkan graphics queue.
		* @return The vulkan graphics queue.
		*/
		[[nodiscard]] static VkQueue GetGraphicsQueue() { return sGraphicsQueue; }

		/*
		* @brief Returns a command buffer for a single use.
		* @return The command buffer created.
		*/
		static const VkCommandBuffer BeginSingleCommandBuffer();

		/*
		* @brief Submits the command buffer to the queue and destroys it.
		* @param aCommandBuffer The command buffer to use and destroy.
		*/
		static void EndSingleCommandBuffer(const VkCommandBuffer aCommandBuffer);

	private:
		/*
		* @brief Returns the required extensions for Vulkan.
		* @return Container with the required extensions.
		*/
		[[nodiscard]] const std::vector<const char*> GetRequiredExtensions() const;

		/*
		* @brief Checks if the given physical device is suitable.
		* @param aDevice The physical device.
		* @return True if the physical device can be used, false otherwise.
		*/
		bool IsDeviceSuitable(VkPhysicalDevice aDevice) const;
		
		/*
		* @brief Checks if its possible to create the swapchain with a given physical device.
		* @param aDevice The physical device.
		* @return True if its possible to create the swapchain with that physical device, false otherwise.
		*/
		bool CanCreateSwapchain(VkPhysicalDevice aDevice) const;

		/*
		* @brief Finds the indices for the graphics and present queues.
		* @return The graphics and present queue indices.
		*/
		const QueueFamilyIndices FindQueueFamilies() const;

		/*
		* @brief Creates the pipeline layout.
		*/
		void CreatePipelineLayout();

	private:
		static VkInstance sInstance; // Instance of the vulkan instance.
#ifdef ILG_DEBUG
		VkDebugUtilsMessengerEXT mDebugMessenger; // Instance of the vulkan debugger messenger.
#endif
		static VkSurfaceKHR sSurface; // Instance of the vulkan surface.
		static VkPhysicalDevice sPhysicalDevice; // Instance of the vulkan physical device.
		static VkDevice sLogicalDevice; // Instance of the vulkan logical device.

		static QueueFamilyIndices sIndices; // The indices of the graphics and present queue.
		static SwapchainSupportDetails sSwapchainSupport; // Swaphcain support information.

		static VkCommandPool sCommandPool; // Instance of the vulkan command pool.

		static VkQueue sGraphicsQueue; // Instance of the vulkan graphics queue.

		static VkDescriptorPool sDescriptorPool; // Instance of the vulkan descriptor pool.
		
		static VkPipelineLayout sPipelineLayout; // Instance of the vulkan descriptor pool.
		static std::vector<VkDescriptorSetLayout> sDescriptorSetLayouts; // Instance of the vulkan descriptor pool.
	};
}