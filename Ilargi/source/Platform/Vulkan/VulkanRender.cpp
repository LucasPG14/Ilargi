#include "ilargipch.h"

#include "VulkanRender.h"
#include "Renderer/Renderer.h"
#include "VulkanVertexBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanIndexBuffer.h"

namespace Ilargi
{
	VulkanRender::VulkanRender()
	{
	}
	
	VulkanRender::~VulkanRender()
	{
	}
	
	void VulkanRender::SubmitGeometry(const std::shared_ptr<CommandBuffer>& aCommandBuffer, const std::shared_ptr<VertexBuffer>& aVertexBuffer, const std::shared_ptr<IndexBuffer>& aIndexBuffer) const
	{
		Renderer::Submit([aCommandBuffer, aVertexBuffer, aIndexBuffer]()
			{
				uint32_t currentFrame{ Renderer::GetCurrentFrame() };

				auto cmdBuffer{ aCommandBuffer->As<VulkanCommandBuffer>()->GetCurrentCommand(currentFrame) };
				aVertexBuffer->Bind(aCommandBuffer);
				aIndexBuffer->Bind(aCommandBuffer);

				vkCmdDrawIndexed(cmdBuffer, aIndexBuffer->GetCount(), 1, 0, 0, 0);
			});
	}
	
	void VulkanRender::DrawDefault(const std::shared_ptr<CommandBuffer>& aCommandBuffer) const
	{
		Renderer::Submit([aCommandBuffer]()
		{
				uint32_t currentFrame{ Renderer::GetCurrentFrame() };

				auto cmdBuffer{ aCommandBuffer->As<VulkanCommandBuffer>()->GetCurrentCommand(currentFrame) };
				vkCmdDraw(cmdBuffer, 6, 1, 0, 0);
		});
	}
}