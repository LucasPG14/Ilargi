#include "ilargipch.h"

#include "VertexBuffer.h"
#include "Renderer.h"

#include "Platform/Vulkan/VulkanVertexBuffer.h"

namespace Ilargi
{
	std::shared_ptr<VertexBuffer> VertexBuffer::Create(void* data, uint32_t size)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanVertexBuffer>(data, size);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}