#include "ilargipch.h"
#include "ISwapchain.h"

#include "Renderer.h"

#include "Platform/Vulkan/VulkanSwapchain.h"

namespace Ilargi
{
	std::shared_ptr<ISwapchain> ISwapchain::Create()
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanSwapchain>();
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}