#pragma once

#include "ilargipch.h"
#include "Base/Debug.h"

namespace Ilargi
{
	enum class EventType
	{
		NONE = 0,
		WINDOW_CLOSE, WINDOW_RESIZE, WINDOW_DROP,
		KEY_PRESSED, KEY_RELEASED, KEY_TYPED,
		MOUSE_BUTTON_PRESSED, MOUSE_BUTTON_RELEASED,
		MOUSE_MOVE, MOUSE_SCROLL
	};

	class Event
	{
		friend class EventDispatcher;
	public:
		/*
		* @brief Returns the event type.
		* @return The event type.
		*/
		virtual EventType GetEventType() const = 0;

		/*
		* @brief Returns the name of the event.
		* @return The name of the event.
		*/
		virtual const char* GetName() const = 0;

		/*
		* @brief Returns the name of the event as a string.
		* @return The name of the event.
		*/
		virtual std::string ToString() const { return GetName(); }

	public:
		bool mHandled{ false }; // Indicates if the event has been consumed.
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		/*
		* @brief Constructor.
		* @param aEvent Reference to the event that has to be executed.
		*/
		EventDispatcher(Event& aEvent) : mEvent(aEvent) {}

		/*
		* @brief Executes the event.
		* @param aFunc The function that has to execute the event.
		* @return True if the event has been consumed, false otherwise.
		*/
		template<typename T>
		bool Dispatch(EventFn<T> aFunc)
		{
			if (mEvent.GetEventType() == T::GetStaticType())
			{
				mEvent.mHandled = aFunc(*(T*)&mEvent);
				return true;
			}
			return false;
		}
	private:
		Event& mEvent; // The event to execute.
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}