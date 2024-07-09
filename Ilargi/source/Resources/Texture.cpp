#include "ilargipch.h"

#include "Texture.h"
#include "Renderer/Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanTexture.h"

namespace Ilargi
{
	std::shared_ptr<Texture2D> Texture2D::Create(std::filesystem::path filepath)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanTexture2D>(filepath);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
	
	std::shared_ptr<Texture2D> Texture2D::Create(void* data, int width, int height, int channels)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanTexture2D>(data, width, height, channels);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}