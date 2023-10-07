#pragma once

namespace Ilargi
{
	class Swapchain
	{
	public:
		virtual void Destroy() = 0;

		virtual void StartFrame() = 0;
		virtual void EndFrame() = 0;

		static std::shared_ptr<Swapchain> Create();
	};
}