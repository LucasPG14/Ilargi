#include "ilargipch.h"

// Main headers
#include "ImGuiPanel.h"
#include "Renderer/Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanImGuiPanel.h"

namespace Ilargi
{
	std::shared_ptr<ImGuiPanel> ImGuiPanel::Create(GLFWwindow* win, const std::shared_ptr<Swapchain> swapchain)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanImGuiPanel>(win, swapchain);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}