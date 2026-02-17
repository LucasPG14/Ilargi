#pragma once

#include "Renderer/IUniformBuffer.h"
#include "VulkanAllocator.h"

namespace Ilargi
{
	class VulkanUniformBuffer : public IUniformBuffer
	{
	public:
		/*
		* @brief Constructor.
		* @param aSize The size of the uniform buffer data.
		* @param aFramesInFlight The number of uniform buffers needed.
		*/
		VulkanUniformBuffer(uint32_t aSize, uint32_t aFramesInFlight);

		/*
		* @brief Destructor.
		*/
		virtual ~VulkanUniformBuffer();

		/*
		* @copydoc UniformBuffer::Destroy()
		*/
		void Destroy() override;

		/*
		* @copydoc UniformBuffer::SetData()
		*/
		void SetData(void* aData, uint32_t aBinding) override;

		/*
		* Returns the vulkan descriptor set.
		* @return The vulkan descriptor set.
		*/
		[[nodiscard]] const VkDescriptorSet GetDescriptorSet() const;

	private:
		std::vector<VulkanBuffer> mUbos; // Container of the vulkan buffers and allocations
		std::vector<void*> mUniformBuffersMapped; // Container with the data mapped to the uniform buffer.
		std::vector<VkDescriptorSet> mDescriptorSets; // Container of the descriptor sets.

		uint32_t mSize; // The size of the uniform buffer data.
	};
}