#include <ilargipch.h>
#include "Base/EntryPoint.h"

#include "LauncherPanel.h"

namespace Ilargi
{
	class IlargiLauncher : public Application
	{
	public:
		IlargiLauncher(const ApplicationProperties& properties) : Application(properties)
		{
			AddPanel(new LauncherPanel());
		}

		~IlargiLauncher()
		{

		}
	};

	Application* CreateApp(int argc, char* argv[])
	{
		ApplicationProperties properties;
		properties.width = 1080;
		properties.height = 720;
		properties.appName = "Ilargi Launcher";
		properties.fullscreen = true;
		properties.iconPath = "";

		return new IlargiLauncher(properties);
	}
}