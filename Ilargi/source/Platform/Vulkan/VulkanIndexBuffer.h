#pragma once

#include "Renderer/IIndexBuffer.h"
#include "VulkanAllocator.h"

namespace Ilargi
{
	class VulkanIndexBuffer : public IIndexBuffer
	{
	public:
		/*
		* @brief Constructor.
		* @param aData The indices.
		* @param aIndicesCount The number of indices.
		*/
		VulkanIndexBuffer(void* aData, uint32_t aIndicesCount);

		/*
		* @brief Destructor.
		*/
		virtual ~VulkanIndexBuffer();

		/*
		* @copydoc IndexBuffer::Bind().
		*/
		void Bind(const std::shared_ptr<ICommandBuffer>& aCommandBuffer) const override;

		/*
		* @copydoc IndexBuffer::Destroy().
		*/
		void Destroy() override;

		/*
		* @copydoc IndexBuffer::GetCount().
		*/
		[[nodiscard]] const uint32_t GetCount() const override { return mCount; }
	private:
		VulkanBuffer mBuffer; // Vulkan buffer and allocation.
		uint32_t mCount; // Number of indices.
	};
}