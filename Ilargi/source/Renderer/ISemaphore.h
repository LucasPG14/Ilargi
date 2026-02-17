#pragma once

namespace Ilargi
{
	class ISemaphore
	{
	public:

		/*
		* @brief Creates the semaphore.
		* @return An instance of the semaphore created.
		*/
		static std::unique_ptr<ISemaphore> Create();
	};
}