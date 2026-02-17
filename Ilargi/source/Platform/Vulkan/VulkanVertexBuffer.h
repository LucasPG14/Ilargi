#pragma once

#include "Renderer/IVertexBuffer.h"
#include "VulkanAllocator.h"

namespace Ilargi
{
	class VulkanVertexBuffer : public IVertexBuffer
	{
	public:
		/*
		* @brief Constructor.
		* @param aData The vertices to store on the vertex buffer.
		* @param aSize The size of the vertices.
		*/
		VulkanVertexBuffer(void* aData, uint32_t aSize);
		
		/*
		* @brief Destructor.
		*/
		virtual ~VulkanVertexBuffer();

		/*
		* @copydoc VertexBuffer::Bind()
		*/
		void Bind(const std::shared_ptr<ICommandBuffer>& aCommandBuffer) const override;

		/*
		* @copydoc VertexBuffer::Destroy()
		*/
		void Destroy() override;
	private:
		VulkanBuffer mBuffer; // The vulkan buffer and allocation.
	};
}