#include "ilargipch.h"

// Main headers
#include "Window.h"
#include "Input.h"

// Event headers
#include "Events/WindowEvents.h"
#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"

// Other headers
#include "Renderer/Context.h"
#include "Renderer/Swapchain.h"

// 3rd Party headers
#include <GLFW/glfw3.h>

namespace Ilargi
{
	Window::Window(const WindowProperties& props, EventCallback eventCallback)
		: eventFunc(eventCallback), properties(props), context(nullptr)
	{
		int success = glfwInit();

		ILG_ASSERT(success, "Error while initializing the GLFW library");

		ILG_CORE_INFO("GLFW library initialized");

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_MAXIMIZED, properties.fullscreen);

		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

		int monitorX, monitorY;
		glfwGetMonitorPos(primaryMonitor, &monitorX, &monitorY);
		
		window = glfwCreateWindow(properties.width, properties.height, properties.appName.c_str(), nullptr, nullptr);

		ILG_ASSERT(window, "Error while creating the GLFW window");
		
		if (!properties.fullscreen)
			glfwSetWindowPos(window, (int)(monitorX + (videoMode->width - properties.width) * 0.5f), (int)(monitorY + (videoMode->height - properties.height) * 0.5f));

		ILG_CORE_INFO("Window created with name: {0} and size: {1}, {2}", properties.appName, properties.width, properties.height);

		Input::SetWindow(window);

		glfwSetWindowUserPointer(window, this);
		SettingCallbacks();

		context = GraphicsContext::Create(window, properties.appName);
		swapchain = Swapchain::Create();
	}
	
	Window::~Window()
	{
	}

	void Window::Destroy()
	{
		swapchain->Destroy();
		context->Destroy();
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void Window::StartFrame() const
	{
		swapchain->StartFrame();
	}

	void Window::EndFrame() const
	{
		swapchain->EndFrame();
	}
	
	void Window::PollEvents() const
	{
		glfwPollEvents();
	}
	
	void Window::SettingCallbacks() const
	{
		// ---------------------------------Setting the close callback---------------------------------------
		glfwSetWindowCloseCallback(window, [](GLFWwindow* win)
		{
			Window& window = *(Window*)glfwGetWindowUserPointer(win);

			WindowCloseEvent closeEvent;
			window.eventFunc(closeEvent);
		});

		// ---------------------------------Setting the resize callback--------------------------------------
		glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h)
		{
			Window& window = *(Window*)glfwGetWindowUserPointer(win);

			//window.GetSwapchain()->SetWidthAndHeight(w, h);
			
			WindowResizeEvent resizeEvent(w, h);
			window.eventFunc(resizeEvent);
		});

		// ---------------------------------Setting the key callback--------------------------------------
		glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods)
		{
			Window& window = *(Window*)glfwGetWindowUserPointer(win);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				window.eventFunc(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				window.eventFunc(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				window.eventFunc(event);
				break;
			}
			}
		});

		// -----------------------------Setting the mouse button callback---------------------------------
		glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods)
		{
			Window& window = *(Window*)glfwGetWindowUserPointer(win);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				window.eventFunc(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				window.eventFunc(event);
				break;
			}
			}
		});

		// ---------------------------------Setting the drop callback-------------------------------------
		glfwSetDropCallback(window, [](GLFWwindow* win, int pathCount, const char* paths[])
		{
			Window& window = *(Window*)glfwGetWindowUserPointer(win);

			std::vector<std::filesystem::path> eventPaths;
			eventPaths.reserve(pathCount);
			for (int i = 0; i < pathCount; ++i)
			{
				eventPaths.push_back(paths[i]);
			}

			WindowDropEvent event(eventPaths);
			window.eventFunc(event);
		});
	}
}