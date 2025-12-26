#pragma once

#include "Scene/Scene.h"

namespace Ilargi
{
	class SceneHierarchyInspectorPanel
	{
	public:
		SceneHierarchyInspectorPanel();
		~SceneHierarchyInspectorPanel();

		void Render();
		
		Entity GetSelected() { return mSelected; }
		void ResetSelected() { mSelected = entt::null; }

		void SetScene(std::shared_ptr<Scene> aScene) { mScene = aScene; }

	private:
		void DrawInspector();

		void DrawNode(const Entity aEntity, const entt::registry& aWorld);
	private:
		std::shared_ptr<Scene> mScene;

		Entity mSelected;

		bool mIsWindowFocused;
	};
}