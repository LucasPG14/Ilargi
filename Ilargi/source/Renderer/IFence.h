#pragma once

namespace Ilargi
{
	class IFence
	{
	public:
		static std::unique_ptr<IFence> Create();
	};
}