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
	RendererStatistics Renderer::sStats = {};
	int Renderer::sCurrentFrame = 0;
	std::vector<std::function<void()>> Renderer::sQueue = {};

	void Renderer::Init()
	{
		uint32_t data = 0xffffffff;
		sDefaultTexture = Texture2D::Create(&data, 1, 1, 4);

		sShaderLibrary->Init();
	}

	void Renderer::Destroy()
	{
		sDefaultTexture.reset();
	}

	void Renderer::SubmitGeometry(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<StaticMesh> mesh)
	{
		sStats.drawCalls++;
		sStats.numMeshes++;
		sRender->SubmitGeometry(commandBuffer, mesh->GetVertexBuffer(), mesh->GetIndexBuffer());
	}

	void Renderer::DrawDefault(std::shared_ptr<CommandBuffer> commandBuffer)
	{
		sStats.drawCalls++;
		sRender->DrawDefault(commandBuffer);
	}

	void Renderer::RenderQueue()
	{
		for (uint32_t i { 0U }; i < sQueue.size(); ++i)
			sQueue[i]();
		
		sQueue.clear();

		sStats.drawCalls = 0;
		sStats.numMeshes = 0;
	}
}