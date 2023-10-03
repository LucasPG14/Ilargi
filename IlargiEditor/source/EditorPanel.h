#pragma once

#include <Ilargi.h>

namespace Ilargi
{
	class SceneHierarchyInspectorPanel;

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
		std::shared_ptr<Scene> scene;

		std::shared_ptr<CommandBuffer> commandBuffer;
		
		std::shared_ptr<VertexBuffer> vertexBuffer;
		std::shared_ptr<IndexBuffer> indexBuffer;

		std::shared_ptr<Framebuffer> framebuffer;
		std::shared_ptr<RenderPass> renderPass;
		std::shared_ptr<Pipeline> pipeline;

		SceneHierarchyInspectorPanel* hierarchyInspector;

		glm::vec2 viewportSize;
		bool needToUpdateFramebuffer;
	};
}