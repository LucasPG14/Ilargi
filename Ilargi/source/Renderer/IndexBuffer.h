#pragma once

namespace Ilargi
{
	class CommandBuffer;

	class IndexBuffer
	{
	public:
		virtual void Bind(std::shared_ptr<CommandBuffer> commandBuffer) const = 0;
		
		virtual void Destroy() = 0;

		virtual const uint32_t GetCount() const = 0;

		static std::shared_ptr<IndexBuffer> Create(void* data, uint32_t indicesCount);
	};
}