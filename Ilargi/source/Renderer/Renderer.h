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

	struct RendererConfig
	{
		uint32_t maxFrames;
		uint32_t maxAASamples;
		float maxAnisotropy;
	};

	struct RendererStatistics
	{
		uint32_t numMeshes{ 0U };
		uint32_t drawCalls{ 0U };
	};

	using RenderFn = std::function<void()>;

	class Renderer
	{
	public:
		static void Init();
		static void Destroy();

		static void SetConfig(const RendererConfig& aConfig) { sConfig = aConfig; }

		static void SetNewFrame(uint32_t aCurrentFrame) { sCurrentFrame = aCurrentFrame; }

		static void SubmitGeometry(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<StaticMesh> mesh);
		static void DrawDefault(std::shared_ptr<CommandBuffer> commandBuffer);
		
		static std::shared_ptr<Texture2D> GetDefaultTexture() { return sDefaultTexture; }

		static const RendererConfig& GetConfig() { return sConfig; }
		static const RendererStatistics& GetStatistics() { return sStats; }
		static const uint32_t GetCurrentFrame() { return sCurrentFrame; }

		static GraphicsAPI GetGraphicsAPI() { return sGraphicsAPI; }
		
		static std::shared_ptr<ShaderLibrary> GetShaderLibrary() { return sShaderLibrary; }

		static void Submit(RenderFn func) { sQueue.push_back(func); }

		static void RenderQueue();
	private:
		static GraphicsAPI sGraphicsAPI;
		static std::unique_ptr<Render> sRender;
		
		static std::shared_ptr<ShaderLibrary> sShaderLibrary;
		
		static std::shared_ptr<Texture2D> sDefaultTexture;

		static RendererConfig sConfig;
		static RendererStatistics sStats;
		static uint32_t sCurrentFrame;

		static std::vector<RenderFn> sQueue;
	};
}