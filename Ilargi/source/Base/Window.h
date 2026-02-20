#pragma once

struct GLFWwindow;

namespace Ilargi
{
	struct ApplicationProperties;

	class Event;
	class IGraphicsContext;
	class ISwapchain;

	using EventCallback = std::function<void(Event&)>;

	class Window
	{
	public:
		/*
		* @brief The window constructor.
		* @param aProps The properties of the window.
		* @param aEventCallback The callback used to manage the events from the window.
		*/
		Window(const ApplicationProperties& aProps, EventCallback aEventCallback);

		/*
		* @brief The window destructor.
		*/
		virtual ~Window();

		/*
		* @brief Destroys all the information of the window.
		*/
		void Destroy();

		/*
		* @brief Updates the events from GLFW.
		*/
		void PollEvents() const;

		/*
		* @brief Gets the pointer to the GLFW window.
		* @return The pointer to the GLFW window.
		*/
		[[nodiscard]] GLFWwindow* GetWindow() const { return mWindow; }

		/*
		* @brief Sets the maximum size for the window or restores the last one.
		*/
		void MaximizeWindow();

		/*
		* @brief Minimizes the window.
		*/
		void MinimizeWindow();

		/*
		* @brief Sets the window position with the x and y specified.
		* @param aX The new x position for the window.
		* @param aY The new y position for the window.
		*/
		void SetWindowPosition(int aX, int aY);

	private:
		/*
		* @brief Sets all the callbacks needed for the application.
		*/
		void SettingCallbacks() const;
	
	private:
		GLFWwindow* mWindow; // Instance of the GLFW window.

		EventCallback mEventFunc; // The event callback.
	};
}