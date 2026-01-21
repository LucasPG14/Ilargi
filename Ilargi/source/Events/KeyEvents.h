#pragma once

#include "Event.h"

namespace Ilargi
{
	class KeyPressedEvent : public Event
	{
	public:
		/*
		* @brief Constructor.
		* @param aKey The key pressed.
		* @param aCount The number of times the key has been pressed.
		*/
		KeyPressedEvent(int aKey, int aCount) : mKey(aKey), mCount(aCount) {}

		/*
		* @brief Returns the key pressed.
		* @return The key pressed.
		*/
		inline const int GetKey() const { return mKey; }

		/*
		* @brief Returns a string with the key pressed and the number of times pressed.
		* @return A string with the event information.
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << mKey << ", " << mCount;
			return ss.str();
		}

		/*
		* @brief Returns the event type.
		* @return The event type.
		*/
		static EventType GetStaticType() { return EventType::KEY_PRESSED; }

		/*
		* @brief Returns the event type.
		* @return The event type.
		*/
		virtual EventType GetEventType() const override { return GetStaticType(); }
		
		/*
		* @brief Returns the name of the event.
		* @return The name of the event.
		*/
		virtual const char* GetName() const override { return "Key Pressed Event"; }

	private:
		int mKey; // The key that has been pressed.
		int mCount; // The amount of times the key has been pressed.
	};

	class KeyReleasedEvent : public Event
	{
	public:
		/*
		* @brief Constructor.
		* @param aKey The key released.
		*/
		KeyReleasedEvent(int aKey) : mKey(aKey) {}

		/*
		* @brief Returns the key released.
		* @return The key released.
		*/
		inline const int GetKey() const { return mKey; }

		/*
		* @brief Returns a string with the key released.
		* @return A string with the event information.
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << mKey;
			return ss.str();
		}

		/*
		* @brief Returns the event type.
		* @return The event type.
		*/
		static EventType GetStaticType() { return EventType::KEY_RELEASED; }

		/*
		* @brief Returns the event type.
		* @return The event type.
		*/
		virtual EventType GetEventType() const override { return GetStaticType(); }

		/*
		* @brief Returns the name of the event.
		* @return The name of the event.
		*/
		virtual const char* GetName() const override { return "Key Released Event"; }

	private:
		int mKey; // The key released.
	};
}