#pragma once

#include "Shader.h"

namespace Ilargi
{
	enum class GraphicsAPI
	{
		VULKAN = 0,
	};

	class Render;
	class CommandBuffer;
	class VertexBuffer;
	class IndexBuffer;
	class StaticMesh;
	class Texture2D;
	class Material;

	struct RendererConfig
	{
		float maxAnisotropy; 
		uint16_t maxFrames; // The maximum frames in flight.
		uint8_t maxAASamples; // The maximum samples of anti-aliasing supported.
	};

	struct RendererStatistics
	{
		uint32_t numMeshes{ 0U }; // The number of meshes rendered.
		uint32_t drawCalls{ 0U }; // The number of draw calls made.
	};

	using RenderFn = std::function<void()>;

	class Renderer
	{
	public:
		/*
		* @brief Initializes the renderer data.
		*/
		static void Init();

		/*
		* @brief Destroys the renderer data.
		*/
		static void Destroy();

		/*
		* @brief Sets the configuration of the renderer.
		* @param aConfig The configuration of the renderer.
		*/
		static void SetConfig(const RendererConfig& aConfig) { sConfig = aConfig; }

		/*
		* @brief Sets the new frame.
		* @param aCurrentFrame The new frame.
		*/
		static void SetNewFrame(const uint32_t aCurrentFrame) { sCurrentFrame = aCurrentFrame; }

		/*
		* @brief Draws a static mesh.
		* @param aCommandBuffer The command buffer to execute the mesh draw call.
		* @param aMesh The mesh to draw.
		*/
		static void SubmitGeometry(const std::shared_ptr<CommandBuffer>& aCommandBuffer, const std::shared_ptr<StaticMesh>& aMesh);
		
		/*
		* @brief Draws a triangle.
		* @param aCommandBuffer The command buffer to execute the draw call.
		*/
		static void DrawDefault(const std::shared_ptr<CommandBuffer>& aCommandBuffer);
		
		/*
		* @brief Returns the default white texture.
		* @return An instance of the default texture.
		*/
		[[nodiscard]] static const std::shared_ptr<Texture2D>& GetDefaultTexture() { return sDefaultTexture; }

		/*
		* @brief Returns the default normal texture.
		* @return An instance of the default texture.
		*/
		[[nodiscard]] static const std::shared_ptr<Texture2D>& GetDefaultNormalTexture() { return sDefaultNormalTexture; }

		/*
		* @brief Returns the default material.
		* @return An instance of the default material.
		*/
		[[nodiscard]] static const std::shared_ptr<Material>& GetDefaultMaterial() { return sDefaultMaterial; }

		/*
		* @brief Returns the renderer configuration.
		* @return The renderer configuration.
		*/
		[[nodiscard]] static const RendererConfig& GetConfig() { return sConfig; }

		/*
		* @brief Returns the renderer statistics.
		* @return The renderer statistics.
		*/
		[[nodiscard]] static const RendererStatistics& GetStatistics() { return sStats; }

		/*
		* @brief Returns the current frame.
		* @return The current frame.
		*/
		[[nodiscard]] static const uint32_t GetCurrentFrame() { return sCurrentFrame; }

		/*
		* @brief Returns the graphics API.
		* @return The graphics API.
		*/
		[[nodiscard]] static GraphicsAPI GetGraphicsAPI() { return sGraphicsAPI; }
		
		/*
		* @brief Returns a shader by a given name.
		* @param aName The name of the shader.
		* @return The instance of the shader.
		*/
		[[nodiscard]] static const std::shared_ptr<Shader>& GetShader(const std::string& aName) { return sShaderLibrary->Get(aName); }

		/*
		* @brief Adds an action to the render queue.
		* @param aFunc The action added to the queue.
		*/
		static void Submit(RenderFn aFunc) { sRenderQueue.push_back(aFunc); }

		/*
		* @brief Executes the render queue.
		*/
		static void RenderQueue();
	private:
		static std::shared_ptr<Texture2D> sDefaultTexture; // Instance of the default texture.
		static std::shared_ptr<Texture2D> sDefaultNormalTexture; // Instance of the default normal map texture.
		static std::shared_ptr<Material> sDefaultMaterial; // Instance of the default material.
		static std::vector<RenderFn> sRenderQueue; // The render queue.

		static std::unique_ptr<ShaderLibrary> sShaderLibrary; // Instance of the shader library.
		static std::unique_ptr<Render> sRender; // Instance of the render.
		static GraphicsAPI sGraphicsAPI; // The graphics API of the application.

		static RendererConfig sConfig; // The configuration of the renderer.
		static RendererStatistics sStats; // The statistics of the renderer.
		static uint32_t sCurrentFrame; // The current frame of the renderer.
	};
}