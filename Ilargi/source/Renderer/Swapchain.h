#pragma once

namespace Ilargi
{
	class Swapchain : public std::enable_shared_from_this<Swapchain>
	{
	public:
		/*
		* @brief Destroys the swapchain data.
		*/
		virtual void Destroy() = 0;

		/*
		* @brief Starts a new frame.
		*/
		virtual void StartFrame() = 0;

		/*
		* @brief Ends the current frame.
		*/
		virtual void EndFrame() = 0;

		/*
		* @brief Casts the swapchain to the specified template class.
		* @tparam The destination type to which the swapchain will be cast.
		* @return An instance of type 'T' created from the swapchain.
		*/
		template <typename T>
		std::shared_ptr<T> As()
		{
			ILG_STATIC_ASSERT(std::is_base_of<Swapchain, T>::value, "T must be a derived class of Swapchain");

			return std::static_pointer_cast<T>(shared_from_this());
		}

		/*
		* @brief Creates the swapchain.
		* @return An instance of the swapchain created.
		*/
		static std::shared_ptr<Swapchain> Create();
	};
}