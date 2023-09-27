#pragma once

namespace Ilargi
{
	class VertexBuffer
	{
	public:
		virtual void Destroy() = 0;

		static std::shared_ptr<VertexBuffer> Create(void* data, uint32_t size);
	};
}