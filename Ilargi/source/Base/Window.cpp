#include "ilargipch.h"

#include "Window.h"

#include "Events/WindowEvents.h"

#include <GLFW/glfw3.h>

namespace Ilargi
{
	Window::Window(const WindowProperties& props, std::function<void(Event&)> eventCallback) : properties(props), eventFunc(eventCallback)
	{
		int success = glfwInit();

		ILG_ASSERT(success);

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window = glfwCreateWindow(properties.width, properties.height, properties.appName.c_str(), nullptr, nullptr);

		ILG_ASSERT(window);

		glfwSetWindowUserPointer(window, this);

		// ---------------------------------Setting the close callback---------------------------------------
		glfwSetWindowCloseCallback(window, [](GLFWwindow* win)
		{
			Window& window = *(Window*)glfwGetWindowUserPointer(win);

			WindowCloseEvent closeEvent;
			window.eventFunc(closeEvent);
		});

		// Here it handles the size of the framebuffer and if the window is minimized
		glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h)
		{
			Window& window = *(Window*)glfwGetWindowUserPointer(win);
			
			WindowResizeEvent resizeEvent(w, h);
			window.eventFunc(resizeEvent);
		});
	}
	
	Window::~Window()
	{
	}
	
	void Window::PollEvents()
	{
		glfwPollEvents();
	}
}