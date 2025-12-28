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

		void Destroy() override;

		void SetData(void* data) override;

		[[nodiscard]] const void* GetDescriptorSet() const override;

	private:
		uint32_t mSize;

		std::vector<VulkanBuffer> mUbos;
		std::vector<void*> mUniformBuffersMapped;
		std::vector<VkDescriptorSet> mDescriptorSets;
	};
}