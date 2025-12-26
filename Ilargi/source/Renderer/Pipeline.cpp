#include "ilargipch.h"

// Main headers
#include "Pipeline.h"
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanPipeline.h"

namespace Ilargi
{
	std::shared_ptr<Pipeline> Pipeline::Create(const PipelineProperties& aProperties)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanPipeline>(aProperties);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}