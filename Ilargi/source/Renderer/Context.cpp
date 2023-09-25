#include "ilargipch.h"

// Main headers
#include "Context.h"
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanContext.h"

namespace Ilargi
{
	std::shared_ptr<GraphicsContext> GraphicsContext::Create(GLFWwindow* win, std::string_view appName)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanContext>(win, appName);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}