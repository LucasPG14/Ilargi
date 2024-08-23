#include "ilargipch.h"

#include "Texture.h"
#include "Renderer/Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanTexture.h"

namespace Ilargi
{
	std::shared_ptr<Texture2D> Texture2D::Create(std::filesystem::path aFilepath)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanTexture2D>(aFilepath);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
	
	std::shared_ptr<Texture2D> Texture2D::Create(void* aData, int aWidth, int aHeight, int aChannels)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanTexture2D>(aData, aWidth, aHeight, aChannels);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}