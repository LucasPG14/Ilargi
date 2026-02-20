#include "ilargipch.h"
#include "Window.h"

// Main headers
#include "Application.h"
#include "Input.h"

// Event headers
#include "Events/WindowEvents.h"
#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"

// Other headers
#include "Renderer/IGraphicsContext.h"
#include "Renderer/ISwapchain.h"

// 3rd Party headers
#include <GLFW/glfw3.h>
#include <stb_image.h>

namespace Ilargi
{
	Window::Window(const ApplicationProperties& aProps, EventCallback aEventCallback)
		: mEventFunc(aEventCallback)
	{
		int success{ glfwInit() };

		ILG_ASSERT(success, "Error while initializing the GLFW library");

		ILG_CORE_INFO("GLFW library initialized");

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_MAXIMIZED, aProps.fullscreen);
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

		GLFWmonitor* primaryMonitor{ glfwGetPrimaryMonitor() };
		const GLFWvidmode* videoMode{ glfwGetVideoMode(primaryMonitor) };

		int monitorX, monitorY;
		glfwGetMonitorPos(primaryMonitor, &monitorX, &monitorY);
		
		mWindow = glfwCreateWindow(aProps.width, aProps.height, aProps.appName.c_str(), nullptr, nullptr);

		ILG_ASSERT(mWindow, "Error while creating the GLFW window");
		
		glfwShowWindow(mWindow);

		if (!aProps.fullscreen)
			glfwSetWindowPos(mWindow, (int)(monitorX + (videoMode->width - aProps.width) * 0.5f), (int)(monitorY + (videoMode->height - aProps.height) * 0.5f));

		ILG_CORE_INFO("Window created with name: {0} and size: {1}, {2}", aProps.appName, aProps.width, aProps.height);

		GLFWimage icon;
		int channels;
		if (!aProps.iconPath.empty())
		{
			icon.pixels = stbi_load(aProps.iconPath.c_str(), &icon.width, &icon.height, &channels, 4);
			glfwSetWindowIcon(mWindow, 1, &icon);
			stbi_image_free(icon.pixels);
		}

		Input::SetWindow(mWindow);

		glfwSetWindowUserPointer(mWindow, this);
		SettingCallbacks();
	}
	
	Window::~Window()
	{
	}

	void Window::Destroy()
	{
		glfwDestroyWindow(mWindow);
		glfwTerminate();
	}
	
	void Window::PollEvents() const
	{
		glfwPollEvents();
	}
	
	void Window::MaximizeWindow()
	{
		if (glfwGetWindowAttrib(mWindow, GLFW_MAXIMIZED))
			glfwRestoreWindow(mWindow);
		else
			glfwMaximizeWindow(mWindow);
	}

	void Window::MinimizeWindow()
	{
		glfwIconifyWindow(mWindow);
	}

	void Window::SetWindowPosition(int aX, int aY)
	{
		int x {0};
		int y {0};
		glfwGetWindowPos(mWindow, &x, &y);
		glfwSetWindowPos(mWindow, x + aX, y + aY);
	}

	void Window::SettingCallbacks() const
	{
		// ---------------------------------Setting the close callback---------------------------------------
		glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* win)
		{
			Window& window{ *(Window*)glfwGetWindowUserPointer(win) };

			WindowCloseEvent closeEvent;
			window.mEventFunc(closeEvent);
		});

		// ---------------------------------Setting the resize callback--------------------------------------
		glfwSetFramebufferSizeCallback(mWindow, [](GLFWwindow* win, int w, int h)
		{
			Window& window{ *(Window*)glfwGetWindowUserPointer(win) };

			//window.GetSwapchain()->SetWidthAndHeight(w, h);
			
			WindowResizeEvent resizeEvent(w, h);
			window.mEventFunc(resizeEvent);
		});

		// ---------------------------------Setting the key callback--------------------------------------
		glfwSetKeyCallback(mWindow, [](GLFWwindow* win, int key, int scancode, int action, int mods)
		{
			Window& window{ *(Window*)glfwGetWindowUserPointer(win) };

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				window.mEventFunc(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				window.mEventFunc(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				window.mEventFunc(event);
				break;
			}
			}
		});

		// -----------------------------Setting the mouse button callback---------------------------------
		glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* win, int button, int action, int mods)
		{
			Window& window{ *(Window*)glfwGetWindowUserPointer(win) };

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				window.mEventFunc(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				window.mEventFunc(event);
				break;
			}
			}
		});

		// ---------------------------------Setting the drop callback-------------------------------------
		glfwSetDropCallback(mWindow, [](GLFWwindow* win, int pathCount, const char* paths[])
		{
			Window& window{ *(Window*)glfwGetWindowUserPointer(win) };

			std::vector<std::filesystem::path> eventPaths;
			eventPaths.reserve(pathCount);
			for (int i { 0 }; i < pathCount; ++i)
			{
				eventPaths.push_back(paths[i]);
			}

			WindowDropEvent event(eventPaths);
			window.mEventFunc(event);
		});
	}
}