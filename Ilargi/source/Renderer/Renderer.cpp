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
	std::shared_ptr<ShaderLibrary> Renderer::sShaderLibrary{ std::make_shared<ShaderLibrary>() };
	std::shared_ptr<Texture2D> Renderer::sDefaultTexture{ nullptr };
	std::shared_ptr<Material> Renderer::sDefaultMaterial{ nullptr };
	RendererConfig Renderer::sConfig {};
	RendererStatistics Renderer::sStats {};
	uint32_t Renderer::sCurrentFrame{ 0U };
	std::vector<std::function<void()>> Renderer::sQueue {};

	void Renderer::Init()
	{
		uint32_t data{ 0xffffffff };
		sDefaultTexture = Texture2D::Create(&data, 1, 1, 4);

		sShaderLibrary->Init();

		sDefaultMaterial = Material::Create(sShaderLibrary->Get("PBR_Static"), {});
	}

	void Renderer::Destroy()
	{
		sDefaultTexture.reset();
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
		for (uint32_t i { 0U }; i < sQueue.size(); ++i)
			sQueue[i]();
		
		sQueue.clear();

		sStats.drawCalls = 0;
		sStats.numMeshes = 0;
	}
}