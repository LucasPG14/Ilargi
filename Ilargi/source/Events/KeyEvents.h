#pragma once

#include "Event.h"

namespace Ilargi
{
	class KeyPressedEvent : public Event
	{
	public:
		KeyPressedEvent(int k, int c) : key(k), count(c) {}

		inline const int GetKey() const { return key; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << key << ", " << count;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::KEY_PRESSED; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "Key Pressed Event"; }

	private:
		int key;
		int count;
	};

	class KeyReleasedEvent : public Event
	{
	public:
		KeyReleasedEvent(int k) : key(k) {}

		inline const int GetKey() const { return key; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << key;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::KEY_RELEASED; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "Key Released Event"; }

	private:
		int key;
	};
}