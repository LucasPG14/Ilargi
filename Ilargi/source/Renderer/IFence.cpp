#include "ilargipch.h"
#include "IFence.h"

#include "Renderer.h"

#include "Platform/Vulkan/VulkanFence.h"

namespace Ilargi
{
	std::unique_ptr<IFence> IFence::Create()
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_unique<VulkanFence>();
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}