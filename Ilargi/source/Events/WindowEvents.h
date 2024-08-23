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
		WindowResizeEvent(unsigned int aWidth, unsigned int aHeight)
			: mWidth(aWidth), mHeight(aHeight) {}

		inline const unsigned int GetWidth() const { return mWidth; }
		inline const unsigned int GetHeight() const { return mHeight; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << mWidth << ", " << mHeight;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::WINDOW_RESIZE; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "Window Resize Event"; }

	private:
		unsigned int mWidth, mHeight;
	};

	class WindowDropEvent : public Event
	{
	public:
		WindowDropEvent(const std::vector<std::filesystem::path>& aPaths) : mPaths(aPaths) {}

		const std::vector<std::filesystem::path>& GetPaths() const { return mPaths; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowDropEvent: " << mPaths.size() << " files";
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::WINDOW_DROP; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetName() const override { return "Window Drop Event"; }

	private:
		std::vector<std::filesystem::path> mPaths;
	};
}