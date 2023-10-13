#pragma once

#include "Renderer/IndexBuffer.h"
#include "VulkanAllocator.h"

namespace Ilargi
{
	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(void* data, uint32_t indicesCount);
		virtual ~VulkanIndexBuffer();

		void Bind(std::shared_ptr<CommandBuffer> commandBuffer) const override;

		void Destroy() override;

		const uint32_t GetCount() const override { return count; }
	private:
		uint32_t count;

		VulkanBuffer buffer;
	};
}