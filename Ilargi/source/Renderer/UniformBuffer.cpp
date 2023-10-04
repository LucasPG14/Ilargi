#include "ilargipch.h"

#include "UniformBuffer.h"
#include "Renderer.h"

#include "Platform/Vulkan/VulkanUniformBuffer.h"

namespace Ilargi
{
	std::shared_ptr<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t framesInFlight)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanUniformBuffer>(size, framesInFlight);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}