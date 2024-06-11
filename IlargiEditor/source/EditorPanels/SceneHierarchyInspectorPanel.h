#pragma once

#include "Scene/Scene.h"

namespace Ilargi
{
	class SceneHierarchyInspectorPanel
	{
	public:
		SceneHierarchyInspectorPanel(std::shared_ptr<Scene> actualScene);
		~SceneHierarchyInspectorPanel();

		void Render();
		
		Entity GetSelected() { return selected; }
		void ResetSelected() { selected = entt::null; }

	private:
		void DrawInspector();

	private:
		std::shared_ptr<Scene> scene;

		Entity selected;
	};
}