#pragma once

#include "Renderer/VertexBuffer.h"
#include "VulkanAllocator.h"

namespace Ilargi
{
	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(void* data, uint32_t size);
		virtual ~VulkanVertexBuffer();

		void Bind(std::shared_ptr<CommandBuffer> commandBuffer) const override;

		void Destroy() override;

		void Bind(VkCommandBuffer cmdBuffer);
	private:
		Buffer buffer;
	};
}