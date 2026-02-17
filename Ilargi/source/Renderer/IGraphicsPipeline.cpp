#include "ilargipch.h"

// Main headers
#include "IGraphicsPipeline.h"
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanGraphicsPipeline.h"

namespace Ilargi
{
	std::shared_ptr<IGraphicsPipeline> IGraphicsPipeline::Create(const GraphicsPipelineProperties& aProperties)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanGraphicsPipeline>(aProperties);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}