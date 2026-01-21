#pragma once

#include <chrono>
#include "Log.h"

namespace Ilargi
{
	class TimeSystem
	{
	public:
		/*
		* @brief Constructor.
		*/
		TimeSystem()
		{
		}

		/*
		* @brief Updates the timer.
		*/
		void Update()
		{
			auto now = std::chrono::high_resolution_clock::now();
			mRealTime = std::chrono::duration<float>(now - mLastTime).count();
			mLastTime = now;

			mDeltaTime = mRealTime * mTimeScale;
		}

		/*
		* @brief Returns the real delta time.
		* @return The real delta time.
		*/
		inline const float GetRealTime() const { return mRealTime; }

		/*
		* @brief Returns the delta time.
		* @return The delta time.
		*/
		inline const float GetDeltaTime() const { return mDeltaTime; }

		/*
		* @brief Sets the time scale.
		* @param aTimeScale The new time scale.
		*/
		void SetTimeScale(float aTimeScale) { mTimeScale = aTimeScale; }

	private:
		std::chrono::high_resolution_clock::time_point mLastTime; // Stores the last time of the timer.
		float mDeltaTime{ 0.0f }; // The delta time of the game.
		float mRealTime{ 0.0f }; // The real delta time.
		float mTimeScale{ 1.0f }; // The time scale defined.
	};
}