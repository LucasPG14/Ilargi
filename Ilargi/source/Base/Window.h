#pragma once

struct GLFWwindow;

namespace Ilargi
{
	class Event;

	struct WindowProperties
	{
		std::string appName;
		int width;
		int height;
		;
	};

	class Window
	{
	public:
		Window(const WindowProperties& props, std::function<void(Event&)> eventCallback);
		~Window();

		void PollEvents();

	private:

		void SettingCallbacks() const;
	private:
		GLFWwindow* window;

		std::function<void(Event&)> eventFunc;
		WindowProperties properties;
	};
}