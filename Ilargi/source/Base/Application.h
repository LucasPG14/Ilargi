#pragma once



namespace Ilargi
{
	class Window;
	class Event;
	class WindowCloseEvent;
	class WindowResizeEvent;
	class Panel;

	struct ApplicationProperties
	{
		std::string appName;
		int width;
		int height;
	};

	class Application
	{
	public:
		Application(const ApplicationProperties& props);
		~Application();

		void Update() const;
		void CleanUp();

		void AddPanel(Panel* panel);

		void OnEvent(Event& event);
		void CloseApp();
	private:
		bool OnCloseEvent(WindowCloseEvent& event);
		bool OnResizeEvent(WindowResizeEvent& event);

	private:
		bool close;
		bool minimized;
		ApplicationProperties properties;

		std::unique_ptr<Window> window;

		std::vector<Panel*> panels;
	};

	extern Application* CreateApp();
}