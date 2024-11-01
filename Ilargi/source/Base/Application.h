#pragma once

#include "Window.h"

namespace Ilargi
{
	class Event;
	class WindowCloseEvent;
	class WindowResizeEvent;
	class Panel;
	class ImGuiPanel;

	struct ApplicationProperties
	{
		std::string appName;
		int width;
		int height;
		bool fullscreen;
		std::string iconPath;
	};

	class Application
	{
	public:
		Application(const ApplicationProperties& aProps);
		~Application();

		void Update() const;

		void AddPanel(Panel* aPanel);

		void OnEvent(Event& aEvent);
		void CloseApp();

		static Application* Get() { return sApp; }
		Window& GetWindow() { return *mWindow; }
	private:
		bool OnCloseEvent(WindowCloseEvent& event);
		bool OnResizeEvent(WindowResizeEvent& event);

	private:
		static Application* sApp;

		bool mClose;
		bool mMinimized;
		ApplicationProperties mProperties;

		std::unique_ptr<Window> mWindow;
		std::shared_ptr<ImGuiPanel> mImguiPanel;

		std::vector<Panel*> mPanels;
	};

	extern Application* CreateApp(int argc, char* argv[]);
}