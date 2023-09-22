#pragma once

#include "Event.h"

namespace Ilargi
{
	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowCloseEvent: Closing App";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WINDOW_MINIMIZE)
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int w, unsigned int h)
			: width(w), height(h) {}

		inline unsigned int GetWidth() const { return width; }
		inline unsigned int GetHeight() const { return height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << width << ", " << height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WINDOW_RESIZE)

	private:
		unsigned int width, height;
	};
}