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
	
	void VulkanRender::SubmitGeometry(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer)
	{
		uint32_t currentFrame = Renderer::GetCurrentFrame();
		
		auto cmdBuffer = std::static_pointer_cast<VulkanCommandBuffer>(commandBuffer)->GetCurrentCommand(currentFrame);
		vertexBuffer->Bind(commandBuffer);
		indexBuffer->Bind(commandBuffer);

		vkCmdDrawIndexed(cmdBuffer, indexBuffer->GetCount(), 1, 0, 0, 0);
	}
}