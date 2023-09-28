#include "ilargipch.h"

// Main headers
#include "Render.h"
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanRender.h"

namespace Ilargi
{
	std::unique_ptr<Render> Render::Create()
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_unique<VulkanRender>();
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}