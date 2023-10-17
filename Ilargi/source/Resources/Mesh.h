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
		StaticMesh(uint32_t submeshCount);
		virtual ~StaticMesh();

		void AddSubmesh(StaticSubmesh& submesh);

		const std::vector<StaticSubmesh>& GetSubmeshes() const { return submeshes; }
		const ResourceType GetType() const { return ResourceType::MESH; }

		// TODO: This must not be here, change when the resource architecture is done
		vec4& GetColor() { return color; }
		const std::shared_ptr<Material> GetMaterial() const { return material; }

		void CreateMaterial(std::shared_ptr<Shader> shader);

	private:
		std::vector<StaticSubmesh> submeshes;

		std::shared_ptr<Material> material;
		vec4 color = vec4(1.0);
	};
}