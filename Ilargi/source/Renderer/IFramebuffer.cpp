#include "ilargipch.h"

#include "IFramebuffer.h"
#include "Renderer.h"

#include "Platform/Vulkan/VulkanFramebuffer.h"

namespace Ilargi
{
	std::shared_ptr<IFramebuffer> IFramebuffer::Create(const FramebufferProperties& props)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanFramebuffer>(props);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}