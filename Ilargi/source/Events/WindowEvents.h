#pragma once

// Main headers
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

		static EventType GetStaticType() { return EventType::WINDOW_CLOSE; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "Window Close Event"; }
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

		static EventType GetStaticType() { return EventType::WINDOW_RESIZE; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "Window Resize Event"; }

	private:
		unsigned int width, height;
	};

	class WindowDropEvent : public Event
	{
	public:
		WindowDropEvent(const std::vector<std::filesystem::path>& p) : paths(p) {}

		const std::vector<std::filesystem::path>& GetPaths() const { return paths; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowDropEvent: " << paths.size() << " files";
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::WINDOW_CLOSE; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "Window Drop Event"; }

	private:
		std::vector<std::filesystem::path> paths;
	};
}