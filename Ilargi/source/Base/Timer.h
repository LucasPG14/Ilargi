#pragma once

#include <chrono>
#include "Log.h"

namespace Ilargi
{
	class Timer
	{
	public:
		Timer()
		{
			Reset();
		}

		~Timer()
		{
			Stop();
		}

		void Reset()
		{
			start = std::chrono::high_resolution_clock::now();
		}

		void Stop()
		{
			auto end = std::chrono::high_resolution_clock::now();

			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

			ILG_CORE_TRACE("The function took: {0}", duration);
		}

		float Elapsed()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count() * 0.001f * 0.001f * 0.001f;
		}

		float ElapsedMillis()
		{
			return Elapsed() * 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> start;
	};
}