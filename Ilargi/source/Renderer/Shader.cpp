#include "ilargipch.h"

// Main headers
#include "Shader.h"
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanShader.h"

namespace Ilargi
{
	std::shared_ptr<Shader> Shader::Create(std::string_view vert, std::string_view frag)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanShader>(vert, frag);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}