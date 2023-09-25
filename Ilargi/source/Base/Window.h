#pragma once

struct GLFWwindow;

namespace Ilargi
{
	struct WindowProperties
	{
		std::string appName;
		int width;
		int height;
		;
	};

	class Event;
	class GraphicsContext;
	class Swapchain;

	class Window
	{
	public:
		Window(const WindowProperties& props, std::function<void(Event&)> eventCallback);
		virtual ~Window();

		void Destroy();

		void PollEvents();

	private:

		void SettingCallbacks() const;
	private:
		GLFWwindow* window;

		std::function<void(Event&)> eventFunc;
		WindowProperties properties;

		std::shared_ptr<GraphicsContext> context;
		std::shared_ptr<Swapchain> swapchain;
	};
}