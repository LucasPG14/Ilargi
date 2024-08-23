#pragma once

#include "Renderer/VertexBuffer.h"
#include "VulkanAllocator.h"

namespace Ilargi
{
	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(void* aData, uint32_t aSize);
		virtual ~VulkanVertexBuffer();

		void Bind(std::shared_ptr<CommandBuffer> aCommandBuffer) const override;

		void Destroy() override;
	private:
		VulkanBuffer mBuffer;
	};
}