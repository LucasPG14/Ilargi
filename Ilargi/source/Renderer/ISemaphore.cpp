#include "ilargipch.h"
#include "ISemaphore.h"

// Main headers
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanSemaphore.h"

namespace Ilargi
{
	std::unique_ptr<ISemaphore> ISemaphore::Create()
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_unique<VulkanSemaphore>();
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}