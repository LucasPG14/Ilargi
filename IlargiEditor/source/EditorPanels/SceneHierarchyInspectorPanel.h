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

	private:
		void DrawInspector();

	private:
		std::shared_ptr<Scene> scene;

		entt::entity selected;
	};
}