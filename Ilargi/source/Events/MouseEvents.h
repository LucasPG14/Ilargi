#pragma once

#include "Event.h"

namespace Ilargi
{
	class MouseButtonPressedEvent : public Event
	{
	public:
		MouseButtonPressedEvent(int btn) : button(btn) {}

		inline const int GetButton() const { return button; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << button;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MOUSE_BUTTON_PRESSED; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "Mouse Button Pressed Event"; }

	private:
		int button;
	};

	class MouseButtonReleasedEvent : public Event
	{
	public:
		MouseButtonReleasedEvent(int btn) : button(btn) {}

		inline int GetButton() const { return button; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << button;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MOUSE_BUTTON_RELEASED; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "Mouse Button Released Event"; }

	private:
		int button;
	};
}