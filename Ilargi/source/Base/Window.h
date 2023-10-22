#pragma once

struct GLFWwindow;

namespace Ilargi
{
	struct WindowProperties
	{
		std::string appName = "";
		int width = 0;
		int height = 0;
		bool fullscreen = false;
	};

	class Event;
	class GraphicsContext;
	class Swapchain;

	using EventCallback = std::function<void(Event&)>;

	class Window
	{
	public:
		Window(const WindowProperties& props, EventCallback eventCallback);
		virtual ~Window();

		void Destroy();

		void StartFrame() const;
		void EndFrame() const;

		void PollEvents() const;

		GLFWwindow* GetWindow() const { return window; }
		const std::shared_ptr<Swapchain> GetSwapchain() const { return swapchain; }

	private:

		void SettingCallbacks() const;
	private:
		GLFWwindow* window;

		EventCallback eventFunc;
		WindowProperties properties;

		std::shared_ptr<GraphicsContext> context;
		std::shared_ptr<Swapchain> swapchain;
	};
}