#pragma once

#include "ImGUI/ImGuiPanel.h"
#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace Ilargi
{
	class VulkanSwapchain;
	class VulkanCommandBuffer;

	class VulkanImGuiPanel : public ImGuiPanel
	{
	public:
		/*
		* @brief Constructor.
		* @param aWindow A pointer to the actual GLFW window.
		* @param aSwapchain A pointer to the swapchain of the window.
		*/
		VulkanImGuiPanel(GLFWwindow* aWindow, const std::shared_ptr<Swapchain>& aSwapchain);
		
		/*
		* @brief Destructor.
		*/
		virtual ~VulkanImGuiPanel();

		/*
		* @copydoc ImGuiPanel::Destroy().
		*/
		void Destroy() const override;

		/*
		* @copydoc ImGuiPanel::Begin().
		*/
		void Begin() const override;

		/*
		* @copydoc ImGuiPanel::End().
		*/
		void End() const override;

	private:
		std::shared_ptr<VulkanSwapchain> mSwapchain; // Instance of the vulkan swapchain.

		VkDescriptorPool mDescriptorPool; // Instance of the descriptor pool.
	};
}