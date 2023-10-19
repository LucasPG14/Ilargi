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
		bool handled = false;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event) : e(event) {}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (e.GetEventType() == T::GetStaticType())
			{
				e.handled = func(*(T*)&e);
				return true;
			}
			return false;
		}
	private:
		Event& e;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}