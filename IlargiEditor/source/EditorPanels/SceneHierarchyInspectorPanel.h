#pragma once

#include "Scene/Scene.h"

namespace Ilargi
{
	class SceneHierarchyInspectorPanel
	{
	public:
		/*
		* @brief Constructor.
		*/
		SceneHierarchyInspectorPanel();

		/*
		* @brief Destructor.
		*/
		~SceneHierarchyInspectorPanel();

		/*
		* @brief Renders the Hierarchy/Inspector panel.
		*/
		void Render();
		
		/*
		* @brief Returns the selected entity.
		* @return The selected entity.
		*/
		[[nodiscard]] const Entity GetSelected() const { return mSelected; }

		/*
		* @brief Resets the selected entity to null.
		*/
		void ResetSelected() { mSelected = entt::null; }

		/*
		* @brief Set the current scene.
		* @param aScene The new scene.
		*/
		void SetScene(std::shared_ptr<Scene> aScene) { mScene = aScene; }

	private:
		/*
		* @brief Draws the ImGui inspector.
		*/
		void DrawInspector();

		/*
		* @brief Draws each node of the hierarchy.
		* @param aEntity The entity to draw.
		* @param aWorld The world of entities.
		*/
		void DrawNode(const Entity aEntity, const entt::registry& aWorld);
	private:
		std::shared_ptr<Scene> mScene; // The scene of the editor.

		Entity mSelected; // The selected entity.

		bool mIsWindowFocused; // Indicates if the window is focused.
	};
}