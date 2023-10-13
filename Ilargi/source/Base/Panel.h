#pragma once

#include <string>

namespace Ilargi
{
	class Event;

	class Panel
	{
	public:
		Panel(const char* pName = "Default Name") : panelName(pName) {}
		~Panel() {}

		virtual void OnInit() = 0;
		virtual void OnDestroy() = 0;

		virtual void Update() = 0;
		virtual void RenderImGui() = 0;
		
		virtual void OnEvent(Event& event) = 0;

	private:
		std::string panelName;
	};
}