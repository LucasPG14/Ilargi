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
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }

	public:
		bool mHandled{ false };
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& aEvent) : mEvent(aEvent) {}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (mEvent.GetEventType() == T::GetStaticType())
			{
				mEvent.mHandled = func(*(T*)&mEvent);
				return true;
			}
			return false;
		}
	private:
		Event& mEvent;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}