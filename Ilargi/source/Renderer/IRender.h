#pragma once

struct GLFWwindow;

namespace Ilargi
{
	class ICommandBuffer;
	class IVertexBuffer;
	class IIndexBuffer;

	class IRender
	{
	public:
		/*
		* @brief Draws a mesh.
		* @param aCommandBuffer The command buffer to make the draw call.
		* @param aVertexBuffer The vertex buffer of the mesh.
		* @param aIndexBuffer The index buffer of the mesh.
		*/
		virtual void SubmitGeometry(const std::shared_ptr<ICommandBuffer>& aCommandBuffer, const std::shared_ptr<IVertexBuffer>& aVertexBuffer, const std::shared_ptr<IIndexBuffer>& aIndexBuffer) const = 0;
		
		/*
		* @brief Draws a triangle.
		* @param aCommandBuffer The command buffer to make the draw call.
		*/
		virtual void DrawDefault(const std::shared_ptr<ICommandBuffer>& aCommandBuffer) const = 0;

		/*
		* @brief Creates the render.
		* @return An instance of the render created.
		*/
		static std::unique_ptr<IRender> Create(GLFWwindow* aWindow, std::string_view aAppName);
	};
}