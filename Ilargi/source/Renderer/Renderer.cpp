#include "ilargipch.h"

#include "Renderer.h"
#include "Render.h"
#include "CommandBuffer.h"
#include "Resources/Mesh.h"

namespace Ilargi
{
	GraphicsAPI Renderer::graphicsAPI = GraphicsAPI::VULKAN;
	std::unique_ptr<Render> Renderer::render = Render::Create();
	RendererConfig Renderer::config = {};
	int Renderer::currentFrame = 0;
	std::vector<std::function<void()>> Renderer::queue = {};
	std::vector<std::shared_ptr<CommandBuffer>> Renderer::submittedCommands = {};
	
	void Renderer::StartFrame()
	{
		currentFrame = (currentFrame + 1) % config.maxFrames;
	}

	void Renderer::SubmitGeometry(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer)
	{
		render->SubmitGeometry(commandBuffer, vertexBuffer, indexBuffer);
	}

	void Renderer::SubmitGeometry(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<StaticMesh> mesh)
	{
		for (auto& submesh : mesh->GetSubmeshes())
		{
			render->SubmitGeometry(commandBuffer, submesh.vertexBuffer, submesh.indexBuffer);
		}
	}

	void Renderer::RenderQueue()
	{
		for (int i = 0; i < queue.size(); ++i)
			queue[i]();
		
		queue.clear();
	}
}