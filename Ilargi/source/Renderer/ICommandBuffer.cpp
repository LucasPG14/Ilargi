#include "ilargipch.h"
#include "ICommandBuffer.h"

// Main headers
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanCommandBuffer.h"

namespace Ilargi
{
	std::shared_ptr<ICommandBuffer> ICommandBuffer::Create(uint32_t aFramesInFlight)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanCommandBuffer>(aFramesInFlight);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}