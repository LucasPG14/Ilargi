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

		void Destroy() override;

		void Bind(VkCommandBuffer cmdBuffer);

	private:
		uint32_t count;

		Buffer buffer;
	};
}