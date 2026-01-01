#pragma once

namespace Ilargi
{
	class CommandBuffer : public std::enable_shared_from_this<CommandBuffer>
	{
	public:
		/*
		* @brief Destroys the command buffer data.
		*/
		virtual void Destroy() const = 0;

		/*
		* @brief Starts a new command.
		*/
		virtual void BeginCommand() const = 0;

		/*
		* @brief Ends the current command.
		*/
		virtual void EndCommand() const = 0;

		/*
		* @brief Submits the actual command to the graphics queue.
		*/
		virtual void Submit() const = 0;

		/*
		* @brief Casts the command buffer to the specified template class.
		* @tparam The destination type to which the command buffer will be cast.
		* @return An instance of type 'T' created from the command buffer.
		*/
		template <typename T>
		std::shared_ptr<T> As()
		{
			ILG_STATIC_ASSERT(std::is_base_of<CommandBuffer, T>::value, "T must be a derived class of CommandBuffer");

			return std::static_pointer_cast<T>(shared_from_this());
		}
		
		/*
		* @brief Creates the command buffer.
		* @param aFramesInFlight The number of command buffers that will be created.
		* @return An instance of the command buffer created.
		*/
		static std::shared_ptr<CommandBuffer> Create(uint32_t aFramesInFlight);
	};
}