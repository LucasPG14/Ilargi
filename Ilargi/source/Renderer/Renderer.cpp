#include "ilargipch.h"

#include "Renderer.h"
#include "Render.h"
#include "CommandBuffer.h"
#include "Resources/Mesh.h"
#include "Resources/Texture.h"
#include "Resources/Material.h"

namespace Ilargi
{
	GraphicsAPI Renderer::sGraphicsAPI{ GraphicsAPI::VULKAN };
	std::unique_ptr<Render> Renderer::sRender{ Render::Create() };
	std::unique_ptr<ShaderLibrary> Renderer::sShaderLibrary{ std::make_unique<ShaderLibrary>() };
	std::unique_ptr<PipelineManager> Renderer::sPipelineManager{ std::make_unique<PipelineManager>() };
	std::unique_ptr<PipelineLayoutManager> Renderer::sPipelineLayoutManager{ std::make_unique<PipelineLayoutManager>() };
	std::unique_ptr<RenderPassManager> Renderer::sRenderPassManager{ std::make_unique<RenderPassManager>() };
	std::shared_ptr<Texture2D> Renderer::sDefaultTexture{ nullptr };
	std::shared_ptr<Texture2D> Renderer::sDefaultNormalTexture{ nullptr };
	std::shared_ptr<Material> Renderer::sDefaultMaterial{ nullptr };
	RendererConfig Renderer::sConfig {};
	RendererStatistics Renderer::sStats {};
	uint32_t Renderer::sCurrentFrame{ 0U };
	std::vector<std::function<void()>> Renderer::sRenderQueue {};

	void Renderer::Init()
	{
		uint32_t data{ 0xffffffff };
		sDefaultTexture = Texture2D::Create(&data, 1, 1, 4);

		uint32_t normalData{ 0x8080FFFF };
		sDefaultNormalTexture = Texture2D::Create(&normalData, 1, 1, 4);
		
		sShaderLibrary->Init();

		sDefaultMaterial = Material::Create(sShaderLibrary->Get("PBR_Static"), {});
		sDefaultMaterial->UpdateTexture("DiffuseMap", sDefaultTexture);
		sDefaultMaterial->UpdateTexture("NormalMap", sDefaultNormalTexture);
	}

	void Renderer::Destroy()
	{
		sDefaultTexture.reset();
		sDefaultNormalTexture.reset();
		sDefaultMaterial.reset();
	}

	void Renderer::SubmitGeometry(const std::shared_ptr<CommandBuffer>& aCommandBuffer, const std::shared_ptr<StaticMesh>& aMesh)
	{
		sStats.drawCalls++;
		sStats.numMeshes++;
		sRender->SubmitGeometry(aCommandBuffer, aMesh->GetVertexBuffer(), aMesh->GetIndexBuffer());
	}

	void Renderer::DrawDefault(const std::shared_ptr<CommandBuffer>& aCommandBuffer)
	{
		sStats.drawCalls++;
		sRender->DrawDefault(aCommandBuffer);
	}

	void Renderer::RenderQueue()
	{
		for (uint32_t i { 0U }; i < sRenderQueue.size(); ++i)
			sRenderQueue[i]();
		
		sRenderQueue.clear();

		sStats.drawCalls = 0;
		sStats.numMeshes = 0;
	}
}