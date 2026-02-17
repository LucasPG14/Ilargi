#include "ilargipch.h"
#include "Mesh.h"

#include "Renderer/IVertexBuffer.h"
#include "Renderer/IIndexBuffer.h"

namespace Ilargi
{
	StaticMesh::StaticMesh(const std::vector<StaticVertex>& aVertices, const std::vector<uint32_t>& aIndices) 
		: mVertices(aVertices), indices(aIndices)
	{
		mVertexBuffer = IVertexBuffer::Create((void*)mVertices.data(), static_cast<uint32_t>(mVertices.size() * sizeof(StaticVertex)));
		mIndexBuffer = IIndexBuffer::Create((void*)indices.data(), static_cast<uint32_t>(indices.size()));
	}
	
	StaticMesh::~StaticMesh()
	{
	}
}