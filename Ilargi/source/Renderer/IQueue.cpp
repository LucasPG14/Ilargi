#include "ilargipch.h"
#include "IQueue.h"

// Main headers
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanQueue.h"

namespace Ilargi
{
	std::unique_ptr<IQueue> IQueue::Create(uint32_t aQueueFamilyIndex)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_unique<VulkanQueue>(aQueueFamilyIndex);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}