#include "ilargipch.h"

#include "Material.h"
#include "Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanMaterial.h"

namespace Ilargi
{
	std::shared_ptr<Material> Material::Create(std::shared_ptr<Shader> aShader)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanMaterial>(aShader);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}