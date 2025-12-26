#pragma once

#include <string>

namespace Ilargi
{
	class Event;

	class Panel
	{
	public:
		Panel(const char* aPanelName = "Default Name") : mPanelName(aPanelName) {}
		~Panel() {}

		virtual void OnInit() = 0;
		virtual void OnDestroy() = 0;

		virtual void Update(float aDeltaTime) = 0;
		virtual void RenderImGui() = 0;
		
		virtual void OnEvent(Event& aEvent) = 0;

	private:
		std::string mPanelName;
	};
}