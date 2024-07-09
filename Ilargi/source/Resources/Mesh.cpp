#include "ilargipch.h"

#include "Mesh.h"
#include "Renderer/Renderer.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Resources/Material.h"

namespace Ilargi
{
	StaticMesh::StaticMesh(const std::vector<StaticVertex>& vert, const std::vector<uint32_t>& ind) 
		: vertices(vert), indices(ind)
	{
		vertexBuffer = VertexBuffer::Create((void*)vertices.data(), static_cast<uint32_t>(vertices.size() * sizeof(StaticVertex)));
		indexBuffer = IndexBuffer::Create((void*)indices.data(), static_cast<uint32_t>(indices.size()));

		material = Material::Create(Renderer::GetShaderLibrary()->Get("PBR_Static"));
	}
	
	StaticMesh::~StaticMesh()
	{
	}
}