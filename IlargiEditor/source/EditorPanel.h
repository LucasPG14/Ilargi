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

		void OnEvent(Event& event) override;

	private:
		void MainMenuBar();

	private:
		std::shared_ptr<Scene> scene;

		std::shared_ptr<CommandBuffer> commandBuffer;

		std::shared_ptr<Framebuffer> framebuffer;
		std::shared_ptr<RenderPass> renderPass;
		std::shared_ptr<Pipeline> pipeline;

		std::shared_ptr<UniformBuffer> uboCamera;
		
		std::shared_ptr<Texture2D> texture;

		EditorCamera camera;
		SceneHierarchyInspectorPanel* hierarchyInspector;
		ResourcesPanel* resourcesPanel;

		glm::vec2 viewportSize;
		bool needToUpdateFramebuffer;

		mat4 constants[2];
	};
}