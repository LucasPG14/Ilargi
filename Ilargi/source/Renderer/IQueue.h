#pragma once

namespace Ilargi
{
	class IQueue
	{
	public:
		static std::unique_ptr<IQueue> Create(uint32_t aQueueFamilyIndex);
	};
}