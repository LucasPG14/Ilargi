#include "ilargipch.h"

// Main headers
#include "Application.h"
#include "Window.h"
#include "Input.h"
#include "Panel.h"
#include "ImGUI/ImGuiPanel.h"

// Debugging tools headers
#include "Debug.h"
#include "Log.h"

// Event headers
#include "Events/Event.h"
#include "Events/WindowEvents.h"

namespace Ilargi
{
	Application::Application(const ApplicationProperties& props) : close(false), minimized(false), properties(props)
	{
		Log::SetClientName(props.appName);

		WindowProperties windowProps;
		windowProps.appName = props.appName;
		windowProps.width = props.width;
		windowProps.height = props.height;
		window = std::make_unique<Window>(windowProps, ILG_BIND_FN(Application::OnEvent));
		imguiPanel = ImGuiPanel::Create(window->GetWindow(), window->GetSwapchain());
	}
	
	Application::~Application()
	{
		imguiPanel->Destroy();
	}
	
	void Application::Update() const
	{
		while (!close)
		{
			window->PollEvents();
			if (minimized)
				continue;

			for (Panel* panel : panels)
				panel->Update();

			window->StartFrame();
			//imguiPanel->Begin();

			//for (Panel* panel : panels)
			//	panel->RenderImGui();

			//imguiPanel->End();
			window->EndFrame();

			window->Present();
		}
	}

	void Application::AddPanel(Panel* panel)
	{
		panels.push_back(panel);
		panel->OnInit();
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowCloseEvent>(ILG_BIND_FN(Application::OnCloseEvent));
		dispatcher.Dispatch<WindowResizeEvent>(ILG_BIND_FN(Application::OnResizeEvent));

		for (Panel* panel : panels)
		{
			if (event.handled)
				break;

			panel->OnEvent(event);
		}
	}

	void Application::CloseApp()
	{
		close = true;
	}
	
	bool Application::OnCloseEvent(WindowCloseEvent& event)
	{
		CloseApp();
		return true;
	}
	
	bool Application::OnResizeEvent(WindowResizeEvent& event)
	{
		if (event.GetWidth() == 0 || event.GetHeight() == 0)
			minimized = true;
		else
			minimized = false;

		return false;
	}
}