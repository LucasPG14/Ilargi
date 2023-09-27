#include "ilargipch.h"

#include "VulkanContext.h"

// 3rd Party headers
#include <GLFW/glfw3.h>

namespace Ilargi
{
	constexpr std::array<const char*, 1> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	constexpr std::array<const char*, 1> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

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
	
	VkInstance VulkanContext::instance = VK_NULL_HANDLE;
	VkSurfaceKHR VulkanContext::surface = VK_NULL_HANDLE;
	VkPhysicalDevice VulkanContext::physicalDevice = VK_NULL_HANDLE;
	VkDevice VulkanContext::logicalDevice = VK_NULL_HANDLE;
	QueueFamilyIndices VulkanContext::indices = {};
	SwapchainSupportDetails VulkanContext::swapchainSupport = {};
	VkCommandPool VulkanContext::commandPool = VK_NULL_HANDLE;
	VkQueue VulkanContext::graphicsQueue = VK_NULL_HANDLE;

	VulkanContext::VulkanContext(GLFWwindow* win, std::string_view appName)
	{
		// Filling aplication info struct to create the instance
		{
			VkApplicationInfo applicationInfo = {};
			applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			applicationInfo.pApplicationName = appName.data();
			applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			applicationInfo.pEngineName = "Ilargi";
			applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			applicationInfo.apiVersion = VK_API_VERSION_1_0;

			VkInstanceCreateInfo instanceInfo = {};
			instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceInfo.pApplicationInfo = &applicationInfo;

			auto extensions = GetRequiredExtensions();
			instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			instanceInfo.ppEnabledExtensionNames = extensions.data();
			
			#ifdef ILG_DEBUG
				instanceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
				instanceInfo.ppEnabledLayerNames = validationLayers.data();
			#else
				instanceInfo.enabledLayerCount = 0;
			#endif

			VK_CHECK_RESULT(vkCreateInstance(&instanceInfo, nullptr, &instance) == VK_SUCCESS, "");
		}

		#ifdef ILG_DEBUG
		// Creating the validation layer debugger
		{
			debugMessenger = VK_NULL_HANDLE;

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
			debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugCreateInfo.pfnUserCallback = VulkanDebugCallback;
			debugCreateInfo.pUserData = nullptr;

			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
			ILG_ASSERT(func != nullptr, "Vulkan function to enable validation layers not found!");
			
			func(instance, &debugCreateInfo, nullptr, &debugMessenger);
		}
		#endif

		// TODO: Maybe this should done in another way if we need to change GLFW to support other platforms
		VK_CHECK_RESULT(glfwCreateWindowSurface(instance, win, nullptr, &surface) == VK_SUCCESS, "Unable to create Vulkan Surface");

		// Creating the physical device
		{
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

			ILG_ASSERT(deviceCount, "Unable to find a GPU with Vulkan support");

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

			for (const auto& device : devices)
			{
				if (IsDeviceSuitable(device))
				{
					physicalDevice = device;
					break;
				}
			}

			ILG_ASSERT(physicalDevice, "Unable to find a suitable GPU!");
		}

		// Creating the logical device
		{
			indices = FindQueueFamilies();

			VkPhysicalDeviceFeatures deviceFeatures = {};

			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

			float queuePriority = 1.0f;
			for (uint32_t queueFamily : uniqueQueueFamilies)
			{
				VkDeviceQueueCreateInfo queueCreateInfo{};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamily;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority;
				queueCreateInfos.push_back(queueCreateInfo);
			}

			VkDeviceCreateInfo deviceInfo = {};
			deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

			deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
			deviceInfo.pQueueCreateInfos = queueCreateInfos.data();

			deviceInfo.pEnabledFeatures = &deviceFeatures;

			deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
			deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

			#ifdef ILG_DEBUG
				deviceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
				deviceInfo.ppEnabledLayerNames = validationLayers.data();
			#else
				deviceInfo.enabledLayerCount = 0;
			#endif
			
			VK_CHECK_RESULT(vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &logicalDevice) == VK_SUCCESS, "Unable to create logical device");
		}

		vkGetDeviceQueue(logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);

		// Creating command pool
		{
			VkCommandPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			poolInfo.queueFamilyIndex = indices.graphicsFamily;

			VK_CHECK_RESULT(vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &commandPool) == VK_SUCCESS, "Unable to create command pool");
		}
	}
	
	VulkanContext::~VulkanContext()
	{
	}
	
	void VulkanContext::Destroy() const
	{
		vkDestroyCommandPool(logicalDevice, commandPool, nullptr);

		vkDestroyDevice(logicalDevice, nullptr);

		vkDestroySurfaceKHR(instance, surface, nullptr);

		#ifdef ILG_DEBUG
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
			ILG_ASSERT(func, "");
			func(instance, debugMessenger, nullptr);
		#endif

		vkDestroyInstance(instance, nullptr);
	}
	
	VkCommandBuffer VulkanContext::BeginSingleCommandBuffer()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void VulkanContext::EndSingleCommandBuffer(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
	}

	std::vector<const char*> VulkanContext::GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		#ifdef ILG_DEBUG
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		#endif

		return extensions;
	}

	bool VulkanContext::IsDeviceSuitable(VkPhysicalDevice device) const
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		//bool extensionsSupported = CheckDeviceExtensionSupport(physicalDevice);

		bool swapChainAdequate = false;
		//if (extensionsSupported)
		//{
		swapchainSupport = QuerySwapchainSupport(device);
		swapChainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
		//}

		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
			deviceFeatures.geometryShader && swapChainAdequate;
	}
	
	SwapchainSupportDetails VulkanContext::QuerySwapchainSupport(VkPhysicalDevice device) const
	{
		SwapchainSupportDetails details;

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
	
	QueueFamilyIndices VulkanContext::FindQueueFamilies()
	{
		QueueFamilyIndices indices = {};
		// Logic to find queue family indices to populate struct with

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

			if (presentSupport)
			{
				indices.presentFamily = i;
			}

			i++;
		}

		return indices;
	}
}