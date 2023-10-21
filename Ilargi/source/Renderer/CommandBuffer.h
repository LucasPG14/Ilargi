#pragma once

namespace Ilargi
{
	class CommandBuffer
	{
	public:

		virtual void Destroy() const = 0;

		virtual void BeginCommand() const = 0;
		virtual void EndCommand() const = 0;

		virtual void Submit() const = 0;
		
		static std::shared_ptr<CommandBuffer> Create(uint32_t framesInFlight);
	};
}