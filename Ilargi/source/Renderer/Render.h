#pragma once

namespace Ilargi
{
	class CommandBuffer;
	class VertexBuffer;
	class IndexBuffer;

	class Render
	{
	public:

		virtual void SubmitGeometry(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer) = 0;

		static std::unique_ptr<Render> Create();
	};
}