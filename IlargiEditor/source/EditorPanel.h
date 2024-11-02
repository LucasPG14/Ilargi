#pragma once

#include <Ilargi.h>

#include "EditorCamera.h"

namespace Ilargi
{
	class SceneHierarchyInspectorPanel;
	class ResourcesPanel;
	
	class VertexBuffer;
	class IndexBuffer;

	class EditorPanel : public Panel
	{
	public:
		EditorPanel();
		~EditorPanel();

		void OnInit() override;
		void OnDestroy() override;

		void Update() override;
		void RenderImGui() override;

		void OnEvent(Event& aEvent) override;

	private:
		void LoadLanguage(std::filesystem::path aFilepath);

		void RenderMainMenuBar();
		void RenderViewport();

		void NewScene();
		void OpenScene();
		void OpenScene(std::string aFilepath);
		void SaveSceneAs();
		void SaveScene(std::string aFilepath);

		bool OnKeyEvent(KeyPressedEvent& aEvent);
	private:
		std::shared_ptr<Scene> mScene;

		std::shared_ptr<CommandBuffer> mCommandBuffer;

		std::shared_ptr<Framebuffer> mFramebuffer;
		std::shared_ptr<RenderPass> mRenderPass;
		std::shared_ptr<Pipeline> mPipeline;

		std::shared_ptr<UniformBuffer> mUBOCamera;

		EditorCamera mCamera;
		SceneHierarchyInspectorPanel* mHierarchyInspector;
		ResourcesPanel* mResourcesPanel;

		glm::vec2 mViewportSize;
		bool mNeedToUpdateFramebuffer;

		glm::mat4 mConstants[2];
		int mOperation;

		// Grid
		std::shared_ptr<RenderPass> mGridRenderPass;
		std::shared_ptr<Pipeline> mGridPipeline;
	};
}