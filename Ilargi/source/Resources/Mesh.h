#pragma once

#include "Resource.h"

#include <vec2.hpp>
#include <vec3.hpp>
#include <vec4.hpp>

namespace Ilargi
{
	class VertexBuffer;
	class IndexBuffer;
	class Material;
	class Shader;

	struct StaticVertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		glm::vec2 texCoord;
	};

	struct StaticSubmesh
	{
		std::vector<StaticVertex> vertices;
		std::vector<uint32_t> indices;

		std::shared_ptr<VertexBuffer> vertexBuffer;
		std::shared_ptr<IndexBuffer> indexBuffer;
	};

	class StaticMesh : public Resource
	{
	public:
		StaticMesh(const std::vector<StaticVertex>& aVertices, const std::vector<uint32_t>& aIndices);
		virtual ~StaticMesh();

		static ResourceType GetStaticType() { return ResourceType::MODEL; }
		const ResourceType GetType() const { return GetStaticType(); }

		const std::shared_ptr<Material> GetMaterial() const { return mMaterial; }
		const std::shared_ptr<VertexBuffer>& GetVertexBuffer() const { return mVertexBuffer; }
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return mIndexBuffer; }

	private:
		std::vector<StaticVertex> mVertices;
		std::vector<uint32_t> indices;

		std::shared_ptr<VertexBuffer> mVertexBuffer;
		std::shared_ptr<IndexBuffer> mIndexBuffer;

		std::shared_ptr<Material> mMaterial;
	};
}