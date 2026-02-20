#include "ilargipch.h"

#include "VulkanRender.h"
#include "Renderer/Renderer.h"
#include "VulkanGraphicsContext.h"
#include "VulkanVertexBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanIndexBuffer.h"

namespace Ilargi
{
	VulkanRender::VulkanRender(GLFWwindow* aWindow, std::string_view aAppName)
	{
		mGraphicsContext = std::make_unique<VulkanGraphicsContext>(aWindow, "Ilargi");
	}
	
	VulkanRender::~VulkanRender()
	{
		mGraphicsContext.reset();
	}
	
	void VulkanRender::SubmitGeometry(const std::shared_ptr<ICommandBuffer>& aCommandBuffer, const std::shared_ptr<IVertexBuffer>& aVertexBuffer, const std::shared_ptr<IIndexBuffer>& aIndexBuffer) const
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
	
	void VulkanRender::DrawDefault(const std::shared_ptr<ICommandBuffer>& aCommandBuffer) const
	{
		Renderer::Submit([aCommandBuffer]()
		{
				uint32_t currentFrame{ Renderer::GetCurrentFrame() };

				auto cmdBuffer{ aCommandBuffer->As<VulkanCommandBuffer>()->GetCurrentCommand(currentFrame) };
				vkCmdDraw(cmdBuffer, 6, 1, 0, 0);
		});
	}
}