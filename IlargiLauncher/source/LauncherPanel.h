#pragma once

#include <Ilargi.h>

namespace Ilargi
{
	class LauncherPanel : public Panel
	{
	public:
		LauncherPanel();
		~LauncherPanel();

		void OnInit() override;
		void OnDestroy() override;

		void Update(float aDeltaTime) override;
		void RenderImGui() override;

		void OnEvent(Event& aEvent) override;

	private:
		std::filesystem::path mDirectory;
	};
}