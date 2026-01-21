#pragma once

#include "Event.h"

namespace Ilargi
{
	class MouseButtonPressedEvent : public Event
	{
	public:
		/*
		* @brief Constructor.
		* @param aButton The mouse button pressed.
		*/
		MouseButtonPressedEvent(int aButton) : mButton(aButton) {}

		/*
		* @brief Returns the mouse button pressed.
		* @return The mouse button pressed.
		*/
		inline const int GetButton() const { return mButton; }

		/*
		* @brief Returns a string with the mouse button pressed.
		* @return A string with the event information.
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << mButton;
			return ss.str();
		}

		/*
		* @brief Returns the event type.
		* @return The event type.
		*/
		static EventType GetStaticType() { return EventType::MOUSE_BUTTON_PRESSED; }
		
		/*
		* @brief Returns the event type.
		* @return The event type.
		*/
		virtual EventType GetEventType() const override { return GetStaticType(); }

		/*
		* @brief Returns the name of the event.
		* @return The name of the event.
		*/
		virtual const char* GetName() const override { return "Mouse Button Pressed Event"; }

	private:
		int mButton; // The mouse button pressed.
	};

	class MouseButtonReleasedEvent : public Event
	{
	public:
		/*
		* @brief Constructor.
		* @param aButton The mouse button released.
		*/
		MouseButtonReleasedEvent(int aButton) : mButton(aButton) {}

		/*
		* @brief Returns the mouse button released.
		* @return The mouse button released.
		*/
		inline int GetButton() const { return mButton; }

		/*
		* @brief Returns a string with the mouse button released.
		* @return A string with the event information.
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << mButton;
			return ss.str();
		}

		/*
		* @brief Returns the event type.
		* @return The event type.
		*/
		static EventType GetStaticType() { return EventType::MOUSE_BUTTON_RELEASED; }
		
		/*
		* @brief Returns the event type.
		* @return The event type.
		*/
		virtual EventType GetEventType() const override { return GetStaticType(); }
		
		/*
		* @brief Returns the name of the event.
		* @return The name of the event.
		*/
		virtual const char* GetName() const override { return "Mouse Button Released Event"; }

	private:
		int mButton; // The button released.
	};
}