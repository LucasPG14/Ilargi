#include "ilargipch.h"

#include "Renderer.h"
#include "Render.h"
#include "CommandBuffer.h"
#include "Resources/Mesh.h"

namespace Ilargi
{
	GraphicsAPI Renderer::graphicsAPI = GraphicsAPI::VULKAN;
	std::unique_ptr<Render> Renderer::render = Render::Create();
	std::shared_ptr<ShaderLibrary> Renderer::shaderLibrary = std::make_shared<ShaderLibrary>();
	RendererConfig Renderer::config = {};
	int Renderer::currentFrame = 0;
	std::vector<std::function<void()>> Renderer::queue = {};
	std::vector<std::shared_ptr<CommandBuffer>> Renderer::submittedCommands = {};

	void Renderer::Init()
	{
		shaderLibrary->Add("shaders/PBR_Static.shader");
		//shaderLibrary->Add("shaders/Grid.shader");
	}

	void Renderer::SubmitGeometry(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<StaticMesh> mesh)
	{
		for (auto& submesh : mesh->GetSubmeshes())
		{
			render->SubmitGeometry(commandBuffer, submesh.vertexBuffer, submesh.indexBuffer);
		}
	}

	void Renderer::DrawDefault(std::shared_ptr<CommandBuffer> commandBuffer)
	{
		render->DrawDefault(commandBuffer);
	}

	void Renderer::RenderQueue()
	{
		for (int i = 0; i < queue.size(); ++i)
			queue[i]();
		
		queue.clear();
	}
}