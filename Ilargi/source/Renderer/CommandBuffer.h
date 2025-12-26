#pragma once

namespace Ilargi
{
	class CommandBuffer : public std::enable_shared_from_this<CommandBuffer>
	{
	public:

		virtual void Destroy() const = 0;

		virtual void BeginCommand() const = 0;
		virtual void EndCommand() const = 0;

		virtual void Submit() const = 0;

		template <typename T>
		std::shared_ptr<T> As()
		{
			ILG_STATIC_ASSERT(std::is_base_of<CommandBuffer, T>::value, "T must be a derived class of CommandBuffer");

			return std::static_pointer_cast<T>(shared_from_this());
		}
		
		static std::shared_ptr<CommandBuffer> Create(uint32_t aFramesInFlight);
	};
}