#pragma once

namespace Ilargi
{
	class UniformBuffer
	{
	public:
		virtual void Destroy() = 0;

		virtual void SetData(void* data) = 0;

		static std::shared_ptr<UniformBuffer> Create(uint32_t size, uint32_t framesInFlight);
	};
}