#include "ilargipch.h"

#include "Skybox.h"
#include "Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanSkybox.h"

namespace Ilargi
{
	std::shared_ptr<Skybox> Skybox::Create(const std::shared_ptr<Shader>& aShader)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanSkybox>(aShader);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}