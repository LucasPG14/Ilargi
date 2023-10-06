#pragma once

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

	class Renderer
	{
	public:

		static void StartFrame();
		static void SetNewFrame(uint32_t index) { currentFrame = index; }

		static void SubmitGeometry(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer);

		static int GetMaxFrames() { return maxFrames; }
		static int GetCurrentFrame() { return currentFrame; }

		static void ResetCurrentFrame() { currentFrame = 0; }

		static void SetMaxFrames(int frms) { maxFrames = frms; }
		static GraphicsAPI GetGraphicsAPI() { return graphicsAPI; }

		static std::vector<std::shared_ptr<CommandBuffer>>& GetSubmittedCommands() { return submittedCommands; }

		static void Submit(std::function<void()> func) { queue.push_back(func); }

		static void AddCommand(std::shared_ptr<CommandBuffer> cmdBuffer) { submittedCommands.push_back(cmdBuffer); }
		static void ClearSubmittedCommands() { submittedCommands.clear(); }

		static void RenderQueue();
	private:
		static GraphicsAPI graphicsAPI;
		static std::unique_ptr<Render> render;

		static int maxFrames;
		static int currentFrame;

		static std::vector<std::function<void()>> queue;
		static std::vector<std::shared_ptr<CommandBuffer>> submittedCommands;
		static std::vector<std::shared_ptr<CommandBuffer>> semaphoresToHandle;
	};
}