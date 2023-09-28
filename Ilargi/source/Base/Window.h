#pragma once

struct GLFWwindow;

namespace Ilargi
{
	struct WindowProperties
	{
		std::string appName = "";
		int width = 0;
		int height = 0;
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

		void StartFrame();
		void EndFrame();

		void Present();

		void PollEvents();

		GLFWwindow* GetWindow() const { return window; }
		const std::shared_ptr<Swapchain> GetSwapchain() const { return swapchain; }

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