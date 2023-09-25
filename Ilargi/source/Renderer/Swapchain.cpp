#include "ilargipch.h"

#include "Swapchain.h"
#include "Renderer.h"

#include "Platform/Vulkan/VulkanSwapchain.h"

namespace Ilargi
{
	std::shared_ptr<Swapchain> Swapchain::Create()
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanSwapchain>();
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}