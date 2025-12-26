#pragma once

namespace Ilargi
{
	class Swapchain : public std::enable_shared_from_this<Swapchain>
	{
	public:
		virtual void Destroy() = 0;

		virtual void StartFrame() = 0;
		virtual void EndFrame() = 0;

		template <typename T>
		std::shared_ptr<T> As()
		{
			ILG_STATIC_ASSERT(std::is_base_of<Swapchain, T>::value, "T must be a derived class of Swapchain");

			return std::static_pointer_cast<T>(shared_from_this());
		}

		static std::shared_ptr<Swapchain> Create();
	};
}