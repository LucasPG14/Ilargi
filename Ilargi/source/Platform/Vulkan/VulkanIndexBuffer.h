#pragma once

#include "Renderer/IndexBuffer.h"
#include "VulkanAllocator.h"

namespace Ilargi
{
	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(void* aData, uint32_t aIndicesCount);
		virtual ~VulkanIndexBuffer();

		void Bind(std::shared_ptr<CommandBuffer> aCommandBuffer) const override;

		void Destroy() override;

		const uint32_t GetCount() const override { return mCount; }
	private:
		uint32_t mCount;

		VulkanBuffer mBuffer;
	};
}