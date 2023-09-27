#include "ilargipch.h"

// Main headers
#include "IndexBuffer.h"
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanIndexBuffer.h"

namespace Ilargi
{
	std::shared_ptr<IndexBuffer> IndexBuffer::Create(void* data, uint32_t indicesCount)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanIndexBuffer>(data, indicesCount);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}