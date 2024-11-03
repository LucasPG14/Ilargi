#include "ilargipch.h"

// Main headers
#include "VulkanContext.h"
#include "VulkanAllocator.h"

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
	
	VkInstance VulkanContext::sInstance = VK_NULL_HANDLE;
	VkSurfaceKHR VulkanContext::sSurface = VK_NULL_HANDLE;
	VkPhysicalDevice VulkanContext::sPhysicalDevice = VK_NULL_HANDLE;
	VkDevice VulkanContext::sLogicalDevice = VK_NULL_HANDLE;
	QueueFamilyIndices VulkanContext::sIndices = {};
	SwapchainSupportDetails VulkanContext::sSwapchainSupport = {};
	VkCommandPool VulkanContext::sCommandPool = VK_NULL_HANDLE;
	VkQueue VulkanContext::sGraphicsQueue = VK_NULL_HANDLE;
	VkDescriptorPool VulkanContext::sDescriptorPool = VK_NULL_HANDLE;

	VulkanContext::VulkanContext(GLFWwindow* aWindow, std::string_view aAppName)
	{
		// Filling aplication info struct to create the instance
		{
			VkApplicationInfo applicationInfo
			{
				VK_STRUCTURE_TYPE_APPLICATION_INFO, // sType
				nullptr,							// pNext
				aAppName.data(),					// pApplicationName
				VK_MAKE_VERSION(1, 0, 0),			// applicationVersion
				"Ilargi",							// pEngineName
				VK_MAKE_VERSION(1, 0, 0),			// engineVersion
				VK_API_VERSION_1_0,					// apiVersion
			};

			auto extensions = GetRequiredExtensions();
			VkInstanceCreateInfo instanceInfo
			{
				VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,				// sType
				nullptr,											// pNext
				0,													// flags
				&applicationInfo,									// pApplicationInfo
				#ifdef ILG_DEBUG
					static_cast<uint32_t>(validationLayers.size()), // enabledLayerCount
					validationLayers.data(),						// ppEnabledLayerNames
				#else
					0,												// enabledLayerCount
					nullptr,										// ppEnabledLayerNames
				#endif
				static_cast<uint32_t>(extensions.size()),			// enabledExtensionCount
				extensions.data()									// ppEnabledExtensionNames
			};

			VK_CHECK_RESULT(vkCreateInstance(&instanceInfo, nullptr, &sInstance));
		}

		#ifdef ILG_DEBUG
		// Creating the validation layer debugger
		{
			mDebugMessenger = VK_NULL_HANDLE;

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo
			{
				VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,												// sType
				nullptr,																								// pNext
				0,																										// flags
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,														// messageSeverity
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |			// messageType
					VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
				VulkanDebugCallback,																					// pfnUserCallback
				nullptr																									// pUserData
			};

			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(sInstance, "vkCreateDebugUtilsMessengerEXT");
			ILG_ASSERT(func != nullptr, "Vulkan function to enable validation layers not found!");
			
			func(sInstance, &debugCreateInfo, nullptr, &mDebugMessenger);
		}
		#endif

		// TODO: Maybe this should be done in another way if we need to change GLFW to support other platforms
		VK_CHECK_RESULT(glfwCreateWindowSurface(sInstance, aWindow, nullptr, &sSurface));

		// Creating the physical device
		{
			uint32_t deviceCount = 0;
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

			VkPhysicalDeviceFeatures deviceFeatures = {};

			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			std::set<uint32_t> uniqueQueueFamilies = { sIndices.graphicsFamily, sIndices.presentFamily };

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

			VkDeviceCreateInfo deviceInfo
			{
				VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,						// sType
				nullptr,													// pNext
				0,															// flags
				static_cast<uint32_t>(queueCreateInfos.size()),				// queueCreateInfoCount
				deviceInfo.pQueueCreateInfos = queueCreateInfos.data(),		// pQueueCreateInfos
				#ifdef ILG_DEBUG
					static_cast<uint32_t>(validationLayers.size()),			// enabledLayerCount
					validationLayers.data(),								// ppEnabledLayerNames
				#else
					0,														// enabledLayerCount
					nullptr,												// ppEnabledLayerNames
				#endif
				static_cast<uint32_t>(deviceExtensions.size()),				// enabledExtensionCount
				deviceExtensions.data(),									// ppEnabledExtensionNames
				&deviceFeatures												// pEnabledFeatures
			};
			
			VK_CHECK_RESULT(vkCreateDevice(sPhysicalDevice, &deviceInfo, nullptr, &sLogicalDevice));
		}

		vkGetDeviceQueue(sLogicalDevice, sIndices.graphicsFamily, 0, &sGraphicsQueue);

		// Creating command pool
		{
			VkCommandPoolCreateInfo poolInfo
			{
				VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,			// sType
				nullptr,											// pNext
				VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,	// flags
				sIndices.graphicsFamily								// queueFamilyIndex
			};

			VK_CHECK_RESULT(vkCreateCommandPool(sLogicalDevice, &poolInfo, nullptr, &sCommandPool));
		}

		VulkanAllocator::Init();

		{
			VkDescriptorPoolSize poolSizes[]
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
			};

			VkDescriptorPoolCreateInfo poolInfo
			{
				VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,		// sType
				nullptr,											// pNext
				VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,	// flags
				1000,												// maxSets
				static_cast<uint32_t>(std::size(poolSizes)),		// poolSizeCount
				poolSizes											// pPoolSizes
			};

			VK_CHECK_RESULT(vkCreateDescriptorPool(sLogicalDevice, &poolInfo, nullptr, &sDescriptorPool));
		}
	}
	
	VulkanContext::~VulkanContext()
	{
	}
	
	void VulkanContext::Destroy() const
	{
		VulkanAllocator::Destroy();

		vkDestroyDescriptorPool(sLogicalDevice, sDescriptorPool, nullptr);

		vkDestroyCommandPool(sLogicalDevice, sCommandPool, nullptr);

		vkDestroyDevice(sLogicalDevice, nullptr);

		vkDestroySurfaceKHR(sInstance, sSurface, nullptr);

		#ifdef ILG_DEBUG
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(sInstance, "vkDestroyDebugUtilsMessengerEXT");
			ILG_ASSERT(func, "");
			func(sInstance, mDebugMessenger, nullptr);
		#endif

		vkDestroyInstance(sInstance, nullptr);
	}
	
	const VkCommandBuffer VulkanContext::BeginSingleCommandBuffer()
	{
		VkCommandBufferAllocateInfo allocInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, // sType
			nullptr,										// pNext
			sCommandPool,									// commandPool
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,				// level
			1,												// commandBufferCount
		};

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(sLogicalDevice, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,	// sType
			nullptr,										// pNext
			VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,	// flags
			nullptr											// pInheritanceInfo
		};

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void VulkanContext::EndSingleCommandBuffer(const VkCommandBuffer aCommandBuffer)
	{
		vkEndCommandBuffer(aCommandBuffer);

		VkSubmitInfo submitInfo
		{
			VK_STRUCTURE_TYPE_SUBMIT_INFO,	// sType
			nullptr,						// pNext
			0,								// waitSemaphoreCount
			nullptr,						// pWaitSemaphores
			nullptr,						// pWaitDstStageMask
			1,								// commandBufferCount
			&aCommandBuffer,				// pCommandBuffers
			0,								// signalSemaphoreCount
			nullptr							// pSignalSemaphores
		};

		vkQueueSubmit(sGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(sGraphicsQueue);

		vkFreeCommandBuffers(sLogicalDevice, sCommandPool, 1, &aCommandBuffer);
	}

	const std::vector<const char*> VulkanContext::GetRequiredExtensions() const
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

	bool VulkanContext::IsDeviceSuitable(VkPhysicalDevice aDevice) const
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(aDevice, &deviceProperties);
		vkGetPhysicalDeviceFeatures(aDevice, &deviceFeatures);

		bool swapChainAdequate = CanCreateSwapchain(aDevice);

		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
			deviceFeatures.geometryShader && swapChainAdequate;
	}
	
	bool VulkanContext::CanCreateSwapchain(VkPhysicalDevice aDevice) const
	{
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(aDevice, sSurface, &formatCount, nullptr);

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(aDevice, sSurface, &presentModeCount, nullptr);

		return formatCount > 0 && presentModeCount > 0;
	}
	
	const QueueFamilyIndices VulkanContext::FindQueueFamilies() const
	{
		QueueFamilyIndices indices = {};
		// Logic to find queue family indices to populate struct with

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(sPhysicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(sPhysicalDevice, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
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