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
		EditorPanel();
		~EditorPanel();

		void OnInit() override;
		void OnDestroy() override;

		void Update(float aDeltaTime) override;
		void RenderImGui() override;

		void OnEvent(Event& aEvent) override;

	private:
		void DrawGrid();
		void DrawGeometry();

		void LoadLanguage(std::filesystem::path aFilepath);

		void RenderMainMenuBar();
		void RenderViewport();

		void NewScene();
		void OpenScene();
		void OpenScene(UUID aUUID);
		void SaveSceneAs();
		void SaveScene(std::string aFilepath);

		bool OnKeyEvent(KeyPressedEvent& aEvent);
	private:
		std::shared_ptr<Scene> mScene;

		std::shared_ptr<CommandBuffer> mCommandBuffer;

		std::shared_ptr<Framebuffer> mFramebuffer;
		std::shared_ptr<RenderPass> mRenderPass;
		std::shared_ptr<Pipeline> mGeometryPipeline;
		std::shared_ptr<Pipeline> mGridPipeline;
		std::shared_ptr<Pipeline> mOutlinePipeline;

		//std::shared_ptr<UniformBuffer> mUBOCamera;

		EditorCamera mCamera;
		SceneHierarchyInspectorPanel* mHierarchyInspector;
		ResourcesPanel* mResourcesPanel;

		glm::vec2 mViewportSize;
		int mOperation;
		bool mNeedToUpdateFramebuffer;

		EditorMode mEditorMode;

		glm::mat4 mStencilMatrix;
	};
}