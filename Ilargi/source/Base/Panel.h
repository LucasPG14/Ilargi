#pragma once

#include <string>

namespace Ilargi
{
	class Event;

	class Panel
	{
	public:
		/*
		* @brief The panel constructor.
		* @param aPanelName The name of the panel. By default is: "Panel".
		*/
		Panel(const char* aPanelName = "Panel") : mPanelName(aPanelName) {}

		/*
		* @brief The panel destructor.
		*/
		~Panel() {}

		/*
		* @brief Initializes the panel.
		*/
		virtual void OnInit() = 0;

		/*
		* @brief Destroys the panel.
		*/
		virtual void OnDestroy() = 0;

		/*
		* @brief Updates the panel.
		* @param aDeltaTime The delta time of the current frame.
		*/
		virtual void Update(float aDeltaTime) = 0;
		
		/*
		* @brief Used to render the ImGui windows.
		*/
		virtual void RenderImGui() = 0;
		
		/*
		* @brief Manage the event received and pass it iterating over the panels container.
		* @param aEvent The event to consume.
		*/
		virtual void OnEvent(Event& aEvent) = 0;

	private:
		std::string mPanelName; // The name of the panel.
	};
}