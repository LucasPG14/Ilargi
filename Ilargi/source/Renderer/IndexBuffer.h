#pragma once

namespace Ilargi
{
	class IndexBuffer
	{
	public:
		virtual void Destroy() = 0;

		static std::shared_ptr<IndexBuffer> Create(void* data, uint32_t indicesCount);
	};
}