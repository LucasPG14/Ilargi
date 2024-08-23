#include "ilargipch.h"

// Main headers
#include "Application.h"
#include "Panel.h"
#include "Renderer/Renderer.h"
#include "ImGUI/ImGuiPanel.h"

// Event headers
#include "Events/Event.h"
#include "Events/WindowEvents.h"

namespace Ilargi
{
	Application* Application::sApp = nullptr;

	Application::Application(const ApplicationProperties& aProps) : mClose(false), mMinimized(false), mProperties(aProps)
	{
		sApp = this;

		Log::SetClientName(aProps.appName);

		WindowProperties windowProps;
		windowProps.appName = aProps.appName;
		windowProps.width = aProps.width;
		windowProps.height = aProps.height;
		windowProps.fullscreen = aProps.fullscreen;
		windowProps.iconPath = "";
		mWindow = std::make_unique<Window>(windowProps, ILG_BIND_FN(Application::OnEvent));
		mImguiPanel = ImGuiPanel::Create(mWindow->GetWindow(), mWindow->GetSwapchain());

		Renderer::Init();
	}
	
	Application::~Application()
	{
		for (Panel* panel : mPanels)
			panel->OnDestroy();

		mImguiPanel->Destroy();
		mWindow->Destroy();
	}
	
	void Application::Update() const
	{
		while (!mClose)
		{
			mWindow->PollEvents();
			if (mMinimized)
				continue;

			for (Panel* panel : mPanels)
				panel->Update();

			Renderer::Submit([this]() { mImguiPanel->Begin(); });
			Renderer::Submit([this]() 
				{
					for (Panel* panel : mPanels)
						panel->RenderImGui();
				});
			Renderer::Submit([this]() { mImguiPanel->End(); });
			
			mWindow->StartFrame();
			Renderer::RenderQueue();
			mWindow->EndFrame();
		}
	}

	void Application::AddPanel(Panel* aPanel)
	{
		mPanels.push_back(aPanel);
		aPanel->OnInit();
	}

	void Application::OnEvent(Event& aEvent)
	{
		EventDispatcher dispatcher(aEvent);

		dispatcher.Dispatch<WindowCloseEvent>(ILG_BIND_FN(Application::OnCloseEvent));
		dispatcher.Dispatch<WindowResizeEvent>(ILG_BIND_FN(Application::OnResizeEvent));

		for (Panel* panel : mPanels)
		{
			if (aEvent.mHandled)
				break;

			panel->OnEvent(aEvent);
		}
	}

	void Application::CloseApp()
	{
		mClose = true;
	}
	
	bool Application::OnCloseEvent(WindowCloseEvent& aEvent)
	{
		CloseApp();
		return true;
	}
	
	bool Application::OnResizeEvent(WindowResizeEvent& aEvent)
	{
		if (aEvent.GetWidth() == 0 || aEvent.GetHeight() == 0)
			mMinimized = true;
		else
			mMinimized = false;

		return false;
	}
}