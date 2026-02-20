#pragma once

#include "IShader.h"
#include "PipelineManager.h"
#include "PipelineLayoutManager.h"
#include "RenderPassManager.h"

struct GLFWwindow;

namespace Ilargi
{
	enum class GraphicsAPI
	{
		VULKAN = 0,
	};

	class IRender;
	class IGraphicsContext;
	class CommandBuffer;
	class VertexBuffer;
	class StaticMesh;
	class Texture2D;
	class Material;
	class IGraphicsPipeline;
	class IPipelineLayout;
	class IDescriptorSetLayout;
	class IRenderPass;

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
		static void Init(GLFWwindow* aWindow, std::string_view aAppName);

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
		static void SubmitGeometry(const std::shared_ptr<ICommandBuffer>& aCommandBuffer, const std::shared_ptr<StaticMesh>& aMesh);
		
		/*
		* @brief Draws a triangle.
		* @param aCommandBuffer The command buffer to execute the draw call.
		*/
		static void DrawDefault(const std::shared_ptr<ICommandBuffer>& aCommandBuffer);
		
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
		[[nodiscard]] static const std::shared_ptr<IShader>& GetShader(const std::string& aName) { return sShaderLibrary->Get(aName); }

		/*
		* @brief Adds an action to the render queue.
		* @param aFunc The action added to the queue.
		*/
		static void Submit(RenderFn aFunc) { sRenderQueue.push_back(aFunc); }

		/*
		* @brief Executes the render queue.
		*/
		static void RenderQueue();

		/*
		* @brief Gets a pipeline with the specified properties.
		* @param aProperties The properties of the pipeline.
		* @return An instance of the pipeline with the given properties.
		*/
		static std::shared_ptr<IGraphicsPipeline> GetPipeline(const GraphicsPipelineProperties& aProperties) { return sPipelineManager->GetPipeline(aProperties); }
		
		/*
		* @brief Gets a pipeline layout with the specified properties.
		* @param aProperties The properties of the pipeline layout.
		* @return An instance of the pipeline layout with the given properties.
		*/
		static std::shared_ptr<IPipelineLayout> GetPipelineLayout(const PipelineLayoutProperties& aProperties) { return sPipelineLayoutManager->GetPipelineLayout(aProperties); }
		
		/*
		* @brief Gets a descriptor set layout with the specified properties.
		* @param aProperties The properties of the descriptor set layout.
		* @return An instance of the descriptor set layout with the given properties.
		*/
		static std::shared_ptr<IDescriptorSetLayout> GetDescriptorSetLayout(const DescriptorSetLayoutProperties& aProperties) { return sPipelineLayoutManager->GetDescriptorSetLayout(aProperties); }
		
		/*
		* @brief Gets a render pass with the specified properties.
		* @param aProperties The properties of the render pass.
		* @return An instance of the render pass with the given properties.
		*/
		static const IRenderPass& GetRenderPass(const RenderPassProperties& aProperties) { return sRenderPassManager->GetRenderPass(aProperties); }
	
	private:
		static std::shared_ptr<Texture2D> sDefaultTexture; // Instance of the default texture.
		static std::shared_ptr<Texture2D> sDefaultNormalTexture; // Instance of the default normal map texture.
		static std::shared_ptr<Material> sDefaultMaterial; // Instance of the default material.
		static std::vector<RenderFn> sRenderQueue; // The render queue.

		static std::unique_ptr<ShaderLibrary> sShaderLibrary; // Instance of the shader library.
		static std::unique_ptr<IRender> sRender; // Instance of the render.
		static std::unique_ptr<PipelineManager> sPipelineManager; // Instance of the pipeline manager.
		static std::unique_ptr<RenderPassManager> sRenderPassManager; // Instance of the render pass manager.
		static std::unique_ptr<PipelineLayoutManager> sPipelineLayoutManager; // Instance of the render pass manager.
		static GraphicsAPI sGraphicsAPI; // The graphics API of the application.

		static RendererConfig sConfig; // The configuration of the renderer.
		static RendererStatistics sStats; // The statistics of the renderer.
		static uint32_t sCurrentFrame; // The current frame of the renderer.
	};
}