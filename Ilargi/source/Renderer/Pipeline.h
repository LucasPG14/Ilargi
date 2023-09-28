#pragma once

namespace Ilargi
{
	class RenderPass;
	class Shader;
	class CommandBuffer;
	class VertexBuffer;
	class IndexBuffer;

	struct PipelineProperties
	{
		std::shared_ptr<Shader> shader;
		std::shared_ptr<RenderPass> renderPass;
	};

	class Pipeline
	{
	public:
		virtual void Destroy() = 0;

		virtual void Bind(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer) = 0;
		virtual void Unbind(std::shared_ptr<CommandBuffer> commandBuffer) = 0;

		static std::shared_ptr<Pipeline> Create(const PipelineProperties& props);
	};
}