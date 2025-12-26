#pragma once

#include "Event.h"

namespace Ilargi
{
	class MouseButtonPressedEvent : public Event
	{
	public:
		MouseButtonPressedEvent(int aButton) : mButton(aButton) {}

		inline const int GetButton() const { return mButton; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << mButton;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MOUSE_BUTTON_PRESSED; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "Mouse Button Pressed Event"; }

	private:
		int mButton;
	};

	class MouseButtonReleasedEvent : public Event
	{
	public:
		MouseButtonReleasedEvent(int aButton) : mButton(aButton) {}

		inline int GetButton() const { return mButton; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << mButton;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MOUSE_BUTTON_RELEASED; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "Mouse Button Released Event"; }

	private:
		int mButton;
	};
}