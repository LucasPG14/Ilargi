#pragma once

namespace Ilargi
{
	class UniformBuffer
	{
	public:
		/*
		* @brief Destroys the uniform buffer data.
		*/
		virtual void Destroy() = 0;

		/*
		* @brief Sets the data of the uniform buffer.
		* @param aData The data to store on the uniform buffer.
		*/
		virtual void SetData(void* aData) = 0;

		/*
		* @brief Gets the ID of the uniform buffer.
		* @return The ID of the uniform buffer.
		*/
		[[nodiscard]] virtual const void* GetDescriptorSet() const = 0;

		/*
		* @brief Creates the uniform buffer.
		* @param aSize The size of the uniform buffer.
		* @param aFramesInFlight The number of uniform buffers IDs needed.
		* @return An instance of the uniform buffer created.
		*/
		static std::shared_ptr<UniformBuffer> Create(uint32_t aSize, uint32_t aFramesInFlight);
	};
}