#include "ilargipch.h"

#include "Renderer.h"
#include "Render.h"
#include "CommandBuffer.h"
#include "Resources/Mesh.h"
#include "Resources/Texture.h"

namespace Ilargi
{
	GraphicsAPI Renderer::sGraphicsAPI = GraphicsAPI::VULKAN;
	std::unique_ptr<Render> Renderer::sRender = Render::Create();
	std::shared_ptr<ShaderLibrary> Renderer::sShaderLibrary = std::make_shared<ShaderLibrary>();
	std::shared_ptr<Texture2D> Renderer::sDefaultTexture = nullptr;
	RendererConfig Renderer::sConfig = {};
	int Renderer::sCurrentFrame = 0;
	std::vector<std::function<void()>> Renderer::sQueue = {};

	void Renderer::Init()
	{
		uint32_t data = 0xffffffff;
		sDefaultTexture = Texture2D::Create(&data, 1, 1, 4);

		sShaderLibrary->Add("Shaders/PBR_Static.shader");
		//shaderLibrary->Add("Shaders/Grid.shader");
	}

	void Renderer::SubmitGeometry(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<StaticMesh> mesh)
	{
		sRender->SubmitGeometry(commandBuffer, mesh->GetVertexBuffer(), mesh->GetIndexBuffer());
	}

	void Renderer::DrawDefault(std::shared_ptr<CommandBuffer> commandBuffer)
	{
		sRender->DrawDefault(commandBuffer);
	}

	void Renderer::RenderQueue()
	{
		for (int i = 0; i < sQueue.size(); ++i)
			sQueue[i]();
		
		sQueue.clear();
	}
}