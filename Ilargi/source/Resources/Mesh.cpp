#include "ilargipch.h"

#include "Mesh.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Material.h"

namespace Ilargi
{
	StaticMesh::StaticMesh(uint32_t submeshCount)
	{
		submeshes.reserve(submeshCount);
	}
	
	StaticMesh::~StaticMesh()
	{
	}
	
	void StaticMesh::AddSubmesh(StaticSubmesh& submesh)
	{
		submesh.vertexBuffer = VertexBuffer::Create((void*)submesh.vertices.data(), static_cast<uint32_t>(submesh.vertices.size() * sizeof(StaticVertex)));
		submesh.indexBuffer = IndexBuffer::Create((void*)submesh.indices.data(), static_cast<uint32_t>(submesh.indices.size()));
		submeshes.push_back(submesh);
	}
	
	void StaticMesh::CreateMaterial(std::shared_ptr<Shader> shader)
	{
		material = Material::Create(shader);
	}
}