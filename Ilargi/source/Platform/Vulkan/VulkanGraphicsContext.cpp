#include "ilargipch.h"

// Main headers
#include "VulkanGraphicsContext.h"
#include "VulkanAllocator.h"
#include "VulkanUtils.h"

// 3rd Party headers
#include <GLFW/glfw3.h>

namespace Ilargi
{
	constexpr std::array<const char*, 1> validationLayers{ "VK_LAYER_KHRONOS_validation" };
	constexpr std::array<const char*, 1> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		{
			ILG_CORE_TRACE("{0}", pCallbackData->pMessage);
			return VK_FALSE;
		}
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		{
			ILG_CORE_INFO("{0}", pCallbackData->pMessage);
			return VK_FALSE;
		}
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		{
			ILG_CORE_WARN("{0}", pCallbackData->pMessage);
			return VK_FALSE;
		}
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		{
			ILG_CORE_ERROR("{0}", pCallbackData->pMessage);
			return VK_FALSE;
		}
		}

		return VK_FALSE;
	}

	VkInstance VulkanGraphicsContext::sInstance{ VK_NULL_HANDLE };
	VkSurfaceKHR VulkanGraphicsContext::sSurface{ VK_NULL_HANDLE };
	VkPhysicalDevice VulkanGraphicsContext::sPhysicalDevice{ VK_NULL_HANDLE };
	VkDevice VulkanGraphicsContext::sLogicalDevice{ VK_NULL_HANDLE };
	QueueFamilyIndices VulkanGraphicsContext::sIndices{};
	SwapchainSupportDetails VulkanGraphicsContext::sSwapchainSupport{};
	VkCommandPool VulkanGraphicsContext::sCommandPool{ VK_NULL_HANDLE };
	VkQueue VulkanGraphicsContext::sGraphicsQueue{ VK_NULL_HANDLE };
	VkDescriptorPool VulkanGraphicsContext::sDescriptorPool{ VK_NULL_HANDLE };

	VulkanGraphicsContext::VulkanGraphicsContext(GLFWwindow* aWindow, std::string_view aAppName)
	{
		// Filling aplication info struct to create the instance
		{
			VkApplicationInfo applicationInfo
			{
				.sType {VK_STRUCTURE_TYPE_APPLICATION_INFO},
				.pNext {nullptr},
				.pApplicationName {aAppName.data()},
				.applicationVersion {VK_MAKE_VERSION(1, 0, 0)},
				.pEngineName {"Ilargi"},
				.engineVersion {VK_MAKE_VERSION(1, 0, 0)},
				.apiVersion {VK_API_VERSION_1_0},
			};

			auto extensions{ GetRequiredExtensions() };
			VkInstanceCreateInfo instanceInfo
			{
				.sType {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO},
				.pNext {nullptr},
				.flags {0U},
				.pApplicationInfo {&applicationInfo},
				#ifdef ILG_DEBUG
					.enabledLayerCount {static_cast<uint32_t>(validationLayers.size())},
					.ppEnabledLayerNames {validationLayers.data()},
				#else
					.enabledLayerCount {0U},
					.ppEnabledLayerNames {nullptr},
				#endif
				.enabledExtensionCount {static_cast<uint32_t>(extensions.size())},
				.ppEnabledExtensionNames {extensions.data()}
			};

			VK_CHECK_RESULT(vkCreateInstance(&instanceInfo, nullptr, &sInstance));
		}

#ifdef ILG_DEBUG
		// Creating the validation layer debugger
		{
			mDebugMessenger = VK_NULL_HANDLE;

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo
			{
				.sType {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT},
				.pNext {nullptr},
				.flags {0U},
				.messageSeverity {VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT},
				.messageType {VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT},
				.pfnUserCallback {VulkanDebugCallback},
				.pUserData {nullptr}
			};

			auto func{ (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(sInstance, "vkCreateDebugUtilsMessengerEXT") };
			ILG_ASSERT(func != nullptr, "Vulkan function to enable validation layers not found!");

			func(sInstance, &debugCreateInfo, nullptr, &mDebugMessenger);
		}
#endif

		// TODO: Maybe this should be done in another way if we need to change GLFW to support other platforms
		VK_CHECK_RESULT(glfwCreateWindowSurface(sInstance, aWindow, nullptr, &sSurface));

		// Creating the physical device
		{
			uint32_t deviceCount{ 0U };
			vkEnumeratePhysicalDevices(sInstance, &deviceCount, nullptr);

			ILG_ASSERT(deviceCount, "Unable to find a GPU with Vulkan support");

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(sInstance, &deviceCount, devices.data());

			for (const auto& device : devices)
			{
				if (IsDeviceSuitable(device))
				{
					sPhysicalDevice = device;
					break;
				}
			}

			ILG_ASSERT(sPhysicalDevice, "Unable to find a suitable GPU!");
		}

		// Creating the logical device
		{
			sIndices = FindQueueFamilies();

			VkPhysicalDeviceFeatures deviceFeatures{};

			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			std::set<uint32_t> uniqueQueueFamilies{ sIndices.graphicsFamily, sIndices.presentFamily };

			float queuePriority{ 1.0f };
			for (uint32_t queueFamily : uniqueQueueFamilies)
			{
				VkDeviceQueueCreateInfo queueCreateInfo{};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamily;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority;
				queueCreateInfos.push_back(queueCreateInfo);
			}

			VkDeviceCreateInfo deviceInfo
			{
				.sType {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO},
				.pNext {nullptr},
				.flags {0U},
				.queueCreateInfoCount {static_cast<uint32_t>(queueCreateInfos.size())},
				.pQueueCreateInfos {queueCreateInfos.data()},
				#ifdef ILG_DEBUG
					.enabledLayerCount {static_cast<uint32_t>(validationLayers.size())},
					.ppEnabledLayerNames {validationLayers.data()},
				#else
					.enabledLayerCount {0U},
					.ppEnabledLayerNames {nullptr},
				#endif
				.enabledExtensionCount {static_cast<uint32_t>(deviceExtensions.size())},
				.ppEnabledExtensionNames {deviceExtensions.data()},
				.pEnabledFeatures {&deviceFeatures}
			};

			VK_CHECK_RESULT(vkCreateDevice(sPhysicalDevice, &deviceInfo, nullptr, &sLogicalDevice));
		}

		vkGetDeviceQueue(sLogicalDevice, sIndices.graphicsFamily, 0, &sGraphicsQueue);

		// Creating command pool
		{
			VkCommandPoolCreateInfo poolInfo
			{
				.sType {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO},
				.pNext {nullptr},
				.flags {VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT},
				.queueFamilyIndex {sIndices.graphicsFamily}
			};

			VK_CHECK_RESULT(vkCreateCommandPool(sLogicalDevice, &poolInfo, nullptr, &sCommandPool));
		}

		VulkanAllocator::Init();

		{
			VkDescriptorPoolSize poolSizes[]
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000U },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000U },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000U },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000U },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000U },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000U },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000U },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000U },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000U },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000U },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000U }
			};

			VkDescriptorPoolCreateInfo poolInfo
			{
				.sType {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO},
				.pNext {nullptr},
				.flags {VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT},
				.maxSets {1000U},
				.poolSizeCount {static_cast<uint32_t>(std::size(poolSizes))},
				.pPoolSizes {poolSizes}
			};

			VK_CHECK_RESULT(vkCreateDescriptorPool(sLogicalDevice, &poolInfo, nullptr, &sDescriptorPool));
		}
	}

	VulkanGraphicsContext::~VulkanGraphicsContext()
	{
	}

	void VulkanGraphicsContext::Destroy() const
	{
		VulkanAllocator::Destroy();

		vkDestroyDescriptorPool(sLogicalDevice, sDescriptorPool, nullptr);

		vkDestroyCommandPool(sLogicalDevice, sCommandPool, nullptr);

		vkDestroyDevice(sLogicalDevice, nullptr);

		vkDestroySurfaceKHR(sInstance, sSurface, nullptr);

#ifdef ILG_DEBUG
		auto func{ (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(sInstance, "vkDestroyDebugUtilsMessengerEXT") };
		ILG_ASSERT(func, "");
		func(sInstance, mDebugMessenger, nullptr);
#endif

		vkDestroyInstance(sInstance, nullptr);
	}

	const VkCommandBuffer VulkanGraphicsContext::BeginSingleCommandBuffer()
	{
		VkCommandBufferAllocateInfo allocInfo
		{
			.sType {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO},
			.pNext {nullptr},
			.commandPool {sCommandPool},
			.level {VK_COMMAND_BUFFER_LEVEL_PRIMARY},
			.commandBufferCount {1U},
		};

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(sLogicalDevice, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo
		{
			.sType {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO},
			.pNext {nullptr},
			.flags {VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT},
			.pInheritanceInfo {nullptr}
		};

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void VulkanGraphicsContext::EndSingleCommandBuffer(const VkCommandBuffer aCommandBuffer)
	{
		vkEndCommandBuffer(aCommandBuffer);

		VkSubmitInfo submitInfo
		{
			.sType {VK_STRUCTURE_TYPE_SUBMIT_INFO},
			.pNext {nullptr},
			.waitSemaphoreCount {0U},
			.pWaitSemaphores {nullptr},
			.pWaitDstStageMask {nullptr},
			.commandBufferCount {1U},
			.pCommandBuffers {&aCommandBuffer},
			.signalSemaphoreCount {0U},
			.pSignalSemaphores {nullptr}
		};

		vkQueueSubmit(sGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(sGraphicsQueue);

		vkFreeCommandBuffers(sLogicalDevice, sCommandPool, 1, &aCommandBuffer);
	}

	const std::vector<const char*> VulkanGraphicsContext::GetRequiredExtensions() const
	{
		uint32_t glfwExtensionCount{ 0U };
		const char** glfwExtensions{ glfwGetRequiredInstanceExtensions(&glfwExtensionCount) };

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef ILG_DEBUG
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		return extensions;
	}

	bool VulkanGraphicsContext::IsDeviceSuitable(VkPhysicalDevice aDevice) const
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(aDevice, &deviceProperties);
		vkGetPhysicalDeviceFeatures(aDevice, &deviceFeatures);

		bool swapChainAdequate{ CanCreateSwapchain(aDevice) };

		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
			deviceFeatures.geometryShader && swapChainAdequate;
	}

	bool VulkanGraphicsContext::CanCreateSwapchain(VkPhysicalDevice aDevice) const
	{
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(aDevice, sSurface, &formatCount, nullptr);

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(aDevice, sSurface, &presentModeCount, nullptr);

		return formatCount > 0 && presentModeCount > 0;
	}

	const QueueFamilyIndices VulkanGraphicsContext::FindQueueFamilies() const
	{
		QueueFamilyIndices indices{};
		// Logic to find queue family indices to populate struct with

		uint32_t queueFamilyCount{ 0U };
		vkGetPhysicalDeviceQueueFamilyProperties(sPhysicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(sPhysicalDevice, &queueFamilyCount, queueFamilies.data());

		int i{ 0 };
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport{ false };
			vkGetPhysicalDeviceSurfaceSupportKHR(sPhysicalDevice, i, sSurface, &presentSupport);

			if (presentSupport)
			{
				indices.presentFamily = i;
			}

			i++;
		}

		return indices;
	}
}