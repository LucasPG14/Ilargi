#pragma once

namespace Ilargi
{
	class CommandBuffer;

	class VertexBuffer
	{
	public:
		/*
		* @brief Binds the vertex buffer.
		* @param aCommandBuffer The command buffer to execute the bind action.
		*/
		virtual void Bind(const std::shared_ptr<CommandBuffer>& aCommandBuffer) const = 0;

		/*
		* @brief Destroys the vertex buffer data.
		*/
		virtual void Destroy() = 0;

		/*
		* @brief Creates the vertex buffer.
		* @param aData The vertices to store in the vertex buffer.
		* @param aSize The size of the data.
		* @return An instance of the vertex buffer created.
		*/
		static std::shared_ptr<VertexBuffer> Create(void* aData, uint32_t aSize);
	};
}