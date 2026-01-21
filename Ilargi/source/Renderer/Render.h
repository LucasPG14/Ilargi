#pragma once

namespace Ilargi
{
	class CommandBuffer;
	class VertexBuffer;
	class IndexBuffer;

	class Render
	{
	public:
		/*
		* @brief Draws a mesh.
		* @param aCommandBuffer The command buffer to make the draw call.
		* @param aVertexBuffer The vertex buffer of the mesh.
		* @param aIndexBuffer The index buffer of the mesh.
		*/
		virtual void SubmitGeometry(const std::shared_ptr<CommandBuffer>& aCommandBuffer, const std::shared_ptr<VertexBuffer>& aVertexBuffer, const std::shared_ptr<IndexBuffer>& aIndexBuffer) const = 0;
		
		/*
		* @brief Draws a triangle.
		* @param aCommandBuffer The command buffer to make the draw call.
		*/
		virtual void DrawDefault(const std::shared_ptr<CommandBuffer>& aCommandBuffer) const = 0;

		/*
		* @brief Creates the render.
		* @return An instance of the render created.
		*/
		static std::unique_ptr<Render> Create();
	};
}