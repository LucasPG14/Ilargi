#include "ilargipch.h"

// Main headers
#include "CommandBuffer.h"
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanCommandBuffer.h"

namespace Ilargi
{
	std::shared_ptr<CommandBuffer> CommandBuffer::Create(uint32_t framesInFlight)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanCommandBuffer>(framesInFlight);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}