#include "ilargipch.h"

// Main headers
#include "RenderPass.h"
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanRenderPass.h"

namespace Ilargi
{
	std::shared_ptr<RenderPass> RenderPass::Create(const RenderPassProperties& props)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanRenderPass>(props);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}