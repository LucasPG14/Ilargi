#pragma once

namespace Ilargi
{
	class UniformBuffer : public std::enable_shared_from_this<UniformBuffer>
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
		* @brief Casts the uniform buffer to the specified template class.
		* @tparam The destination type to which the uniform buffer will be cast.
		* @return An instance of type 'T' created from the uniform buffer.
		*/
		template <typename T>
		std::shared_ptr<T> As()
		{
			ILG_STATIC_ASSERT(std::is_base_of<UniformBuffer, T>::value, "T must be a derived class of UniformBuffer");

			return std::static_pointer_cast<T>(shared_from_this());
		}

		/*
		* @brief Creates the uniform buffer.
		* @param aSize The size of the uniform buffer.
		* @param aFramesInFlight The number of uniform buffers IDs needed.
		* @return An instance of the uniform buffer created.
		*/
		static std::shared_ptr<UniformBuffer> Create(uint32_t aSize, uint32_t aFramesInFlight);
	};
}