#include "ilargipch.h"

#include "VertexBuffer.h"
#include "Renderer.h"

#include "Platform/Vulkan/VulkanVertexBuffer.h"

namespace Ilargi
{
	std::shared_ptr<VertexBuffer> VertexBuffer::Create(void* aData, uint32_t aSize)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanVertexBuffer>(aData, aSize);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}