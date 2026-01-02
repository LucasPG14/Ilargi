#pragma once

#include "TimeSystem.h"
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
		std::string iconPath;
		uint16_t width;
		uint16_t height;
		bool fullscreen;
	};

	class Application
	{
	public:
		/*
		* @brief The application constructor.
		* @param aProps Structure with the data necessary to create the application.
		*/
		Application(const ApplicationProperties& aProps);
		
		/*
		* @brief The application destructor.
		*/
		~Application();

		/*
		* @brief The main loop of the application.
		*/
		void Update();
		
		/*
		* @brief Adds a panel to the panels container.
		* @param aPanel The panel that will be added to the list.
		*/
		void AddPanel(Panel* aPanel);

		/*
		* @brief Manage the event received and pass it iterating over the panels container.
		* @param aEvent The event to consume.
		*/
		void OnEvent(Event& aEvent);
		
		/*
		* @brief Sets the mClose bool to false to close the application.
		*/
		void CloseApp();

		/*
		* @brief Get the actual application.
		* @return Pointer to the application.
		*/
		[[nodiscard]] static Application* Get() { return sApp; }

		/*
		* @brief Get the actual window.
		* @return Reference to the application.
		*/
		[[nodiscard]] Window& GetWindow() { return *mWindow; }
	
	private:
		/*
		* @brief Closes the app.
		* @return True if the event is consumed, false otherwise.
		*/
		bool OnCloseEvent(WindowCloseEvent& aEvent);
		
		/*
		* @brief Manages the resize of the window.
		* @return True if the event is consumed, false otherwise.
		*/
		bool OnResizeEvent(WindowResizeEvent& aEvent);

	private:
		ApplicationProperties mProperties; // The main properties of the application.
		std::vector<Panel*> mPanels; // Container of all the panels of the application.

		std::unique_ptr<Window> mWindow; // Unique pointer of the window.
		std::unique_ptr<ImGuiPanel> mImGuiPanel; // Pointer of the ImGuiPanel(used for the editor).
		static Application* sApp; // Pointer of the application.

		TimeSystem mTimer; // The timer of the application.

		bool mClose; // Used to check if the app has to close.
		bool mMinimized; // Used to know if the app is minimized.
	};

	extern Application* CreateApp(int argc, char* argv[]);
}