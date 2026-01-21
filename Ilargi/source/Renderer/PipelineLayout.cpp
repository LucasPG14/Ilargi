#include "ilargipch.h"

// Main headers
#include "PipelineLayout.h"
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanPipelineLayout.h"

namespace Ilargi
{
	std::shared_ptr<PipelineLayout> PipelineLayout::Create(const PipelineLayoutProperties& aProperties)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanPipelineLayout>(aProperties);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}