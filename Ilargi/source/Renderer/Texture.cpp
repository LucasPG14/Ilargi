#include "ilargipch.h"

#include "Texture.h"
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanTexture.h"

namespace Ilargi
{
	std::shared_ptr<Texture2D> Texture2D::Create(void* data, uint32_t width, uint32_t height)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanTexture2D>(data, width, height);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}