#pragma once

namespace Ilargi
{
	class CommandBuffer;

	class VertexBuffer
	{
	public:
		virtual void Bind(std::shared_ptr<CommandBuffer> commandBuffer) const = 0;

		virtual void Destroy() = 0;

		static std::shared_ptr<VertexBuffer> Create(void* data, uint32_t size);
	};
}