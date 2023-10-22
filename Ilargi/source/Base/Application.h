#pragma once

namespace Ilargi
{
	class Window;
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
	};

	class Application
	{
	public:
		Application(const ApplicationProperties& props);
		~Application();

		void Update() const;

		void AddPanel(Panel* panel);

		void OnEvent(Event& event);
		void CloseApp();

		static Application* Get() { return app; }
	private:
		bool OnCloseEvent(WindowCloseEvent& event);
		bool OnResizeEvent(WindowResizeEvent& event);

	private:
		static Application* app;

		bool close;
		bool minimized;
		ApplicationProperties properties;

		std::unique_ptr<Window> window;
		std::shared_ptr<ImGuiPanel> imguiPanel;

		std::vector<Panel*> panels;
	};

	extern Application* CreateApp(int argc, char* argv[]);
}