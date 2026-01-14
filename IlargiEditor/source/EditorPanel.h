#pragma once

#include <Ilargi.h>

#include "EditorCamera.h"

namespace Ilargi
{
	class SceneHierarchyInspectorPanel;
	class ResourcesPanel;
	
	class VertexBuffer;
	class IndexBuffer;

	enum class EditorMode
	{
		EDITOR = 0,
		PLAY = 1,
		PAUSE = 2
	};

	class EditorPanel : public Panel
	{
	public:
		/*
		* @brief Constructor.
		*/
		EditorPanel();

		/*
		* @brief Destructor.
		*/
		~EditorPanel();

		/*
		* @copydoc Panel::OnInit()
		*/
		void OnInit() override;

		/*
		* @copydoc Panel::OnDestroy()
		*/
		void OnDestroy() override;

		/*
		* @copydoc Panel::Update()
		*/
		void Update(float aDeltaTime) override;
		
		/*
		* @copydoc Panel::RenderImGui()
		*/
		void RenderImGui() override;

		/*
		* @copydoc Panel::OnEvent()
		*/
		void OnEvent(Event& aEvent) override;

	private:
		/*
		* @brief Draws the editor grid.
		*/
		void DrawGrid();

		/*
		* @brief Draws all the models of the scene.
		*/
		void DrawGeometry();

		/*
		* @brief Draws outline of the selected entity.
		*/
		void DrawOutline();

		/*
		* @brief Renders the ImGui main menu bar.
		*/
		void RenderMainMenuBar();
		
		/*
		* @brief Renders the viewport.
		*/
		void RenderViewport();

		/*
		* @brief Creates a new scene.
		*/
		void NewScene();

		/*
		* @brief Opens a scene.
		*/
		void OpenScene();

		/*
		* @brief Opens a scene by a given identifier.
		* @param aUUID The scene identifier.
		*/
		void OpenScene(UUID aUUID);

		/*
		* @brief Save the scene as a Ilargi file.
		*/
		void SaveSceneAs();

		/*
		* @brief Save the scene to a given filepath.
		* @param aFilepath The filepath of the scene.
		*/
		void SaveScene(std::string aFilepath);

		/*
		* @brief Process the input of a key.
		* @param aEvent The key pressed event information.
		*/
		bool OnKeyEvent(KeyPressedEvent& aEvent);
	private:
		std::shared_ptr<Scene> mScene; // Instance of the scene.
		std::shared_ptr<CommandBuffer> mCommandBuffer; // Instance of the editor command buffer.

		std::shared_ptr<Framebuffer> mFramebuffer; // Instance of the editor framebuffer.
		std::shared_ptr<RenderPass> mRenderPass; // Instance of the editor render pass.
		std::shared_ptr<Pipeline> mGeometryPipeline; // Instance of the geometry pipeline.
		std::shared_ptr<Pipeline> mGridPipeline; // Instance of the grid pipeline.
		std::shared_ptr<Pipeline> mOutlinePipeline; // Instance of the outline pipeline.

		std::shared_ptr<RenderPass> mMousePickingRenderPass; // Instance of the render pass for mouse picking.
		std::shared_ptr<Framebuffer> mMousePickingFramebuffer; // Instance of the framebuffer for mouse picking.
		std::shared_ptr<Pipeline> mMousePickingPipeline; // Instance of the pipeline for mouse picking.

		EditorCamera mCamera; // Instance of the editor camera.
		SceneHierarchyInspectorPanel* mHierarchyInspector; // Instance of the hierarchy/inspector panel.
		ResourcesPanel* mResourcesPanel; // Instance of the resources panel.

		glm::vec2 mViewportSize; // The size of the viewport.
		glm::vec2 mViewportPosition; // The position of the viewport.
		int mOperation; // The mode of the ImGuizmo.
		bool mNeedToUpdateFramebuffer; // Indicates if the framebuffer has to be updated.

		EditorMode mEditorMode; // Instance of the current editor mode.

		glm::mat4 mStencilMatrix; // The world matrix of the selected entity to draw the outline.
	};
}