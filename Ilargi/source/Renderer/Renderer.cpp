#include "ilargipch.h"

#include "Renderer.h"
#include "Render.h"
#include "CommandBuffer.h"

namespace Ilargi
{
	GraphicsAPI Renderer::graphicsAPI = GraphicsAPI::VULKAN;
	std::unique_ptr<Render> Renderer::render = Render::Create();
	int Renderer::maxFrames = 0;
	int Renderer::currentFrame = 0;
	std::vector<std::function<void()>> Renderer::queue = {};
	std::vector<std::shared_ptr<CommandBuffer>> Renderer::submittedCommands = {};
	
	void Renderer::StartFrame()
	{
		currentFrame = (currentFrame + 1) % maxFrames;
	}

	void Renderer::SubmitGeometry(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer)
	{
		render->SubmitGeometry(commandBuffer, vertexBuffer, indexBuffer);
	}

	void Renderer::RenderQueue()
	{
		for (int i = 0; i < queue.size(); ++i)
			queue[i]();
		
		queue.clear();
	}
}