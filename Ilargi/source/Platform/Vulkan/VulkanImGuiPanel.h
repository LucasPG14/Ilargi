#pragma once

#include "ImGUI/ImGuiPanel.h"
#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Ilargi
{
	class Swapchain;
	class VulkanSwapchain;
	class VulkanCommandBuffer;

	class VulkanImGuiPanel : public ImGuiPanel
	{
	public:
		VulkanImGuiPanel(GLFWwindow* win, const std::shared_ptr<Swapchain> swapchain);
		virtual ~VulkanImGuiPanel();

		void Destroy() const override;

		void Begin() const override;
		void End() const override;

	private:
		std::shared_ptr<VulkanSwapchain> vkSwapchain;
		std::shared_ptr<VulkanCommandBuffer> commandBuffer;

		VkDescriptorPool descriptorPool;
	};
}