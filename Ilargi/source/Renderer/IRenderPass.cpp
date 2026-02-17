#include "ilargipch.h"

// Main headers
#include "IRenderPass.h"
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanRenderPass.h"

namespace Ilargi
{
	std::unique_ptr<IRenderPass> IRenderPass::Create(const RenderPassProperties& aProperties)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_unique<VulkanRenderPass>(aProperties);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
}