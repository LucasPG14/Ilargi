#pragma once

#include "Base/Panel.h"

namespace Ilargi
{
	class EditorPanel : public Panel
	{
	public:
		EditorPanel();
		~EditorPanel();

		void OnInit() override;
		void Update() override;
		void OnDestroy() override;

		void OnEvent(Event& event) override;

	private:
	};
}