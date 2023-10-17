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

	struct RendererConfig
	{
		uint32_t maxFrames;
		uint32_t maxAASamples;
		float maxAnisotropy;
	};

	struct RendererData
	{
		//mat4 viewProj;
	};

	class Renderer
	{
	public:
		static void Init();

		static void SetConfig(const RendererConfig& conf) { config = conf; }

		static void SetNewFrame(uint32_t index) { currentFrame = index; }

		static void SubmitGeometry(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<StaticMesh> mesh);

		static const RendererConfig& GetConfig() { return config; }
		static const int GetCurrentFrame() { return currentFrame; }

		static GraphicsAPI GetGraphicsAPI() { return graphicsAPI; }
		
		static std::shared_ptr<ShaderLibrary> GetShaderLibrary() { return shaderLibrary; }

		static std::vector<std::shared_ptr<CommandBuffer>>& GetSubmittedCommands() { return submittedCommands; }

		static void Submit(std::function<void()> func) { queue.push_back(func); }

		static void AddCommand(std::shared_ptr<CommandBuffer> cmdBuffer) { submittedCommands.push_back(cmdBuffer); }
		static void ClearSubmittedCommands() { submittedCommands.clear(); }

		static void RenderQueue();
	private:
		static GraphicsAPI graphicsAPI;
		static std::unique_ptr<Render> render;
		
		static std::shared_ptr<ShaderLibrary> shaderLibrary;

		static RendererConfig config;
		static int currentFrame;

		static std::vector<std::function<void()>> queue;
		static std::vector<std::shared_ptr<CommandBuffer>> submittedCommands;
		static std::vector<std::shared_ptr<CommandBuffer>> semaphoresToHandle;
	};
}