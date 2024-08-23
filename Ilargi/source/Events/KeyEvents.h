#pragma once

#include "Event.h"

namespace Ilargi
{
	class KeyPressedEvent : public Event
	{
	public:
		KeyPressedEvent(int aKey, int aCount) : mKey(aKey), mCount(aCount) {}

		inline const int GetKey() const { return mKey; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << mKey << ", " << mCount;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::KEY_PRESSED; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "Key Pressed Event"; }

	private:
		int mKey;
		int mCount;
	};

	class KeyReleasedEvent : public Event
	{
	public:
		KeyReleasedEvent(int aKey) : mKey(aKey) {}

		inline const int GetKey() const { return mKey; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << mKey;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::KEY_RELEASED; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "Key Released Event"; }

	private:
		int mKey;
	};
}