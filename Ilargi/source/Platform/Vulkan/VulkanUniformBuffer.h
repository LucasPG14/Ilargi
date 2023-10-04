#pragma once

#include "Renderer/UniformBuffer.h"
#include "VulkanAllocator.h"

namespace Ilargi
{
	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(uint32_t size, uint32_t framesInFlight);
		virtual ~VulkanUniformBuffer();

		void SetData(void* data) override;

	private:
		uint32_t size;

		std::vector<Buffer> ubos;
		std::vector<void*> uniformBuffersMapped;
	};
}