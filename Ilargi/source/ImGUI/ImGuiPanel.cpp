#include "ilargipch.h"

// Main headers
#include "ImGuiPanel.h"
#include "Renderer/Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanImGuiPanel.h"

namespace Ilargi
{
	std::unique_ptr<ImGuiPanel> ImGuiPanel::Create(GLFWwindow* aWindow, const std::shared_ptr<Swapchain>& aSwapchain)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_unique<VulkanImGuiPanel>(aWindow, aSwapchain);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}