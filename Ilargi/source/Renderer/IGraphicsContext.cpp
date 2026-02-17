#include "ilargipch.h"

// Main headers
#include "IGraphicsContext.h"
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanGraphicsContext.h"

namespace Ilargi
{
	std::unique_ptr<IGraphicsContext> IGraphicsContext::Create(GLFWwindow* win, std::string_view appName)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_unique<VulkanGraphicsContext>(win, appName);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}