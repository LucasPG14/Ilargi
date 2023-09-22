#include <ilargipch.h>
#include <Ilargi.h>

#include "EditorPanel.h"

namespace Ilargi
{
	class IlargiEditor : public Application
	{
	public:
		IlargiEditor(const ApplicationProperties& properties) : Application(properties)
		{
			AddPanel(new EditorPanel());
		}
		
		~IlargiEditor()
		{

		}
	};

	Application* CreateApp()
	{
		ApplicationProperties properties;
		properties.width = 1080;
		properties.height = 720;
		properties.appName = "Ilargi Editor";

		return new IlargiEditor(properties);
	}
}