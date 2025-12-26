#pragma once

struct GLFWwindow;

namespace Ilargi
{
	struct WindowProperties
	{
		std::string appName{ "" };
		int width{ 0 };
		int height{ 0 };
		bool fullscreen{ false };
		std::string iconPath{ "" };
	};

	class Event;
	class GraphicsContext;
	class Swapchain;

	using EventCallback = std::function<void(Event&)>;

	class Window
	{
	public:
		Window(const WindowProperties& aProps, EventCallback aEventCallback);
		virtual ~Window();

		void Destroy();

		void StartFrame() const;
		void EndFrame() const;

		void PollEvents() const;

		GLFWwindow* GetWindow() const { return mWindow; }
		const std::shared_ptr<Swapchain> GetSwapchain() const { return mSwapchain; }

	private:

		void SettingCallbacks() const;
	private:
		GLFWwindow* mWindow;

		EventCallback mEventFunc;
		WindowProperties mProperties;

		std::shared_ptr<GraphicsContext> mContext;
		std::shared_ptr<Swapchain> mSwapchain;
	};
}