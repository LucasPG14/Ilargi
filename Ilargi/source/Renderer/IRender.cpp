#include "ilargipch.h"
#include "IRender.h"

// Main headers
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanRender.h"

namespace Ilargi
{
	std::unique_ptr<IRender> IRender::Create()
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_unique<VulkanRender>();
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}