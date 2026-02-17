#include "ilargipch.h"
#include "IUniformBuffer.h"

#include "Renderer.h"

#include "Platform/Vulkan/VulkanUniformBuffer.h"

namespace Ilargi
{
	std::shared_ptr<IUniformBuffer> IUniformBuffer::Create(uint32_t aSize, uint32_t aFramesInFlight)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanUniformBuffer>(aSize, aFramesInFlight);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}