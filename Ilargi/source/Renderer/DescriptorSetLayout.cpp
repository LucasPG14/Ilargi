#include "ilargipch.h"

// Main headers
#include "DescriptorSetLayout.h"
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanDescriptorSetLayout.h"

namespace Ilargi
{
	std::shared_ptr<DescriptorSetLayout> DescriptorSetLayout::Create(const DescriptorSetLayoutProperties& aProperties)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanDescriptorSetLayout>(aProperties);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}