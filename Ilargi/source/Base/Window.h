#pragma once

struct GLFWwindow;

namespace Ilargi
{
	struct WindowProperties
	{
		std::string appName{ "" };
		std::string iconPath{ "" };
		int width{ 0 };
		int height{ 0 };
		bool fullscreen{ false };
	};

	class Event;
	class GraphicsContext;
	class Swapchain;

	using EventCallback = std::function<void(Event&)>;

	class Window
	{
	public:
		/*
		* @brief The window constructor.
		* @param aProps The properties of the window.
		* @param aEventCallback The callback used to manage the events from the window.
		*/
		Window(const WindowProperties& aProps, EventCallback aEventCallback);

		/*
		* @brief The window destructor.
		*/
		virtual ~Window();

		/*
		* @brief Destroys all the information of the window.
		*/
		void Destroy();

		/*
		* @brief Used to start the frame of the swapchain.
		*/
		void StartFrame() const;

		/*
		* @brief Used to end the frame of the swapchain.
		*/
		void EndFrame() const;

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
		* @brief Gets the pointer to the swapchain.
		* @return The swapchain pointer.
		*/
		[[nodiscard]] const std::shared_ptr<Swapchain> GetSwapchain() const { return mSwapchain; }

	private:
		/*
		* @brief Sets all the callbacks needed for the application.
		*/
		void SettingCallbacks() const;
	
	private:
		WindowProperties mProperties; // The window properties.
		GLFWwindow* mWindow; // Instance of the GLFW window.
		std::unique_ptr<GraphicsContext> mContext; // Instance of the graphics context.

		std::shared_ptr<Swapchain> mSwapchain; // Instance of the swapchain.

		EventCallback mEventFunc; // The event callback.
	};
}