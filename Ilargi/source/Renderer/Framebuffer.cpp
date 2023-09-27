#include "ilargipch.h"

#include "Framebuffer.h"
#include "Renderer.h"

#include "Platform/Vulkan/VulkanFramebuffer.h"

namespace Ilargi
{
	std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferProperties& props)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanFramebuffer>(props);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}