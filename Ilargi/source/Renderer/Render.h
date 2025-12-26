#pragma once

namespace Ilargi
{
	class CommandBuffer;
	class VertexBuffer;
	class IndexBuffer;
	class Material;

	class Render
	{
	public:
		virtual void SubmitGeometry(std::shared_ptr<CommandBuffer> aCommandBuffer, std::shared_ptr<VertexBuffer> aVertexBuffer, std::shared_ptr<IndexBuffer> aIndexBuffer) const = 0;
		virtual void DrawDefault(std::shared_ptr<CommandBuffer> aCommandBuffer) const = 0;

		static std::unique_ptr<Render> Create();
	};
}