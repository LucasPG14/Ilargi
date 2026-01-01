#pragma once

// Main headers
#include "Event.h"

namespace Ilargi
{
	class WindowCloseEvent : public Event
	{
	public:
		/*
		* @brief Constructor.
		*/
		WindowCloseEvent() {}

		/*
		* @brief Returns a string notifying that the app is going to close.
		* @return A string with the event information.
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowCloseEvent: Closing App";
			return ss.str();
		}

		/*
		* @brief Returns the event type.
		* @return The event type.
		*/
		static EventType GetStaticType() { return EventType::WINDOW_CLOSE; }

		/*
		* @brief Returns the event type.
		* @return The event type.
		*/
		virtual EventType GetEventType() const override { return GetStaticType(); }

		/*
		* @brief Returns the name of the event.
		* @return The name of the event.
		*/
		virtual const char* GetName() const override { return "Window Close Event"; }
	};

	class WindowResizeEvent : public Event
	{
	public:
		/*
		* @brief Constructor.
		* @param aWidth The width of the window.
		* @param aHeight The height of the window.
		*/
		WindowResizeEvent(unsigned int aWidth, unsigned int aHeight)
			: mWidth(aWidth), mHeight(aHeight) {}

		/*
		* @brief Returns the width of the window.
		* @return The width of the window.
		*/
		inline const unsigned int GetWidth() const { return mWidth; }

		/*
		* @brief Returns the height of the window.
		* @return The height of the window.
		*/
		inline const unsigned int GetHeight() const { return mHeight; }

		/*
		* @brief Returns a string with the new width and height of the window.
		* @return A string with the event information.
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << mWidth << ", " << mHeight;
			return ss.str();
		}

		/*
		* @brief Returns the event type.
		* @return The event type.
		*/
		static EventType GetStaticType() { return EventType::WINDOW_RESIZE; }
		
		/*
		* @brief Returns the event type.
		* @return The event type.
		*/
		virtual EventType GetEventType() const override { return GetStaticType(); }

		/*
		* @brief Returns the name of the event.
		* @return The name of the event.
		*/
		virtual const char* GetName() const override { return "Window Resize Event"; }

	private:
		unsigned int mWidth; // The width of the window.
		unsigned int mHeight; // The height of the window.
	};

	class WindowDropEvent : public Event
	{
	public:
		/*
		* @brief Constructor.
		* @param aPaths The paths of all the files dropped.
		*/
		WindowDropEvent(const std::vector<std::filesystem::path>& aPaths) : mPaths(aPaths) {}

		/*
		* @brief Returns the paths of all the files dropped.
		* @return The paths of the files dropped.
		*/
		const std::vector<std::filesystem::path>& GetPaths() const { return mPaths; }

		/*
		* @brief Returns a string with the number of files dropped.
		* @return A string with the event information.
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowDropEvent: " << mPaths.size() << " files";
			return ss.str();
		}

		/*
		* @brief Returns the event type.
		* @return The event type.
		*/
		static EventType GetStaticType() { return EventType::WINDOW_DROP; }

		/*
		* @brief Returns the event type.
		* @return The event type.
		*/
		virtual EventType GetEventType() const override { return GetStaticType(); }

		/*
		* @brief Returns the name of the event.
		* @return The name of the event.
		*/
		virtual const char* GetName() const override { return "Window Drop Event"; }

	private:
		std::vector<std::filesystem::path> mPaths; // Container with the paths of all the files dropped.
	};
}