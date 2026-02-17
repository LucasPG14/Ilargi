#pragma once

namespace Ilargi
{
	class ICommandBuffer;

	class IIndexBuffer
	{
	public:
		/*
		* @brief Binds the index buffer in the command buffer passed as a parameter.
		* @param aCommandBuffer The command buffer that executes the binding of the index buffer.
		*/
		virtual void Bind(const std::shared_ptr<ICommandBuffer>& aCommandBuffer) const = 0;
		
		/*
		* @brief Destroys the index buffer.
		*/
		virtual void Destroy() = 0;

		/*
		* @brief Returns the number of indices.
		* @return The number of indices.
		*/
		[[nodiscard]] virtual const uint32_t GetCount() const = 0;

		/*
		* @brief Creates the index buffer.
		* @param aData The indices.
		* @param aIndicesCount The number of indices.
		* @return An instance of the index buffer created.
		*/
		static std::shared_ptr<IIndexBuffer> Create(void* aData, uint32_t aIndicesCount);
	};
}