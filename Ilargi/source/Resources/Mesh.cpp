#include "ilargipch.h"

#include "Mesh.h"
#include "Renderer/Renderer.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Resources/Material.h"

namespace Ilargi
{
	StaticMesh::StaticMesh(const std::vector<StaticVertex>& aVertices, const std::vector<uint32_t>& aIndices) 
		: mVertices(aVertices), indices(aIndices)
	{
		sizeof(StaticVertex);
		mVertexBuffer = VertexBuffer::Create((void*)mVertices.data(), static_cast<uint32_t>(mVertices.size() * sizeof(StaticVertex)));
		mIndexBuffer = IndexBuffer::Create((void*)indices.data(), static_cast<uint32_t>(indices.size()));
	}
	
	StaticMesh::~StaticMesh()
	{
	}
}