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
		StaticMesh(uint32_t submeshCount);
		virtual ~StaticMesh();

		void AddSubmesh(StaticSubmesh& submesh);

		const std::vector<StaticSubmesh>& GetSubmeshes() const { return submeshes; }
		const ResourceType GetType() const { return ResourceType::MESH; }

		// TODO: This must not be here, change when the resource architecture is done
		glm::vec4& GetColor() { return color; }
		const std::shared_ptr<Material> GetMaterial() const { return material; }

		void CreateMaterial(std::shared_ptr<Shader> shader);

	private:
		std::vector<StaticSubmesh> submeshes;

		std::shared_ptr<Material> material;
		glm::vec4 color = glm::vec4(1.0);
	};
}