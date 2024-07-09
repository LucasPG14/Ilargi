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

	void Renderer::Init()
	{
		shaderLibrary->Add("Shaders/PBR_Static.shader");
		//shaderLibrary->Add("Shaders/Grid.shader");
	}

	void Renderer::SubmitGeometry(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<StaticMesh> mesh)
	{
		render->SubmitGeometry(commandBuffer, mesh->GetVertexBuffer(), mesh->GetIndexBuffer());
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