#pragma once

#include "Resource.h"

#include <Utils/Math/Vec2.h>
#include <Utils/Math/Vec3.h>
#include <Utils/Math/Vec4.h>

namespace Ilargi
{
	class VertexBuffer;
	class IndexBuffer;
	class Material;
	class Shader;

	struct StaticVertex
	{
		vec3 position;
		vec3 normal;
		vec3 tangent;
		vec3 bitangent;
		vec2 texCoord;
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
		StaticMesh(const std::vector<StaticVertex>& vert, const std::vector<uint32_t>& ind);
		virtual ~StaticMesh();

		const ResourceType GetType() const { return ResourceType::MESH; }

		const std::shared_ptr<Material> GetMaterial() const { return material; }
		const std::shared_ptr<VertexBuffer>& GetVertexBuffer() const { return vertexBuffer; }
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return indexBuffer; }

	private:
		std::vector<StaticVertex> vertices;
		std::vector<uint32_t> indices;

		std::shared_ptr<VertexBuffer> vertexBuffer;
		std::shared_ptr<IndexBuffer> indexBuffer;

		std::shared_ptr<Material> material;
	};
}