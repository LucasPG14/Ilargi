#include "ilargipch.h"

// Main headers
#include "IIndexBuffer.h"
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanIndexBuffer.h"

namespace Ilargi
{
	std::shared_ptr<IIndexBuffer> IIndexBuffer::Create(void* data, uint32_t indicesCount)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanIndexBuffer>(data, indicesCount);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}