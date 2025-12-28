#pragma once

namespace Ilargi
{
	class UniformBuffer
	{
	public:
		virtual void Destroy() = 0;

		virtual void SetData(void* aData) = 0;
		[[nodiscard]] virtual const void* GetDescriptorSet() const = 0;

		static std::shared_ptr<UniformBuffer> Create(uint32_t aSize, uint32_t aFramesInFlight);
	};
}