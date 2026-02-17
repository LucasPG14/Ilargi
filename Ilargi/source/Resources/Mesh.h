#pragma once

#include "Resource.h"

#include <vec2.hpp>
#include <vec3.hpp>
#include <vec4.hpp>

namespace Ilargi
{
	class IVertexBuffer;
	class IIndexBuffer;

	struct AABB
	{
		glm::vec3 min;
		glm::vec3 max;
	};

	// TODO: Check half floats for normal and tangent
	struct StaticVertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec4 tangent;
		glm::vec2 texCoord;
	};

	struct StaticSubmesh
	{
		UUID mesh;
		UUID material;
	};

	class StaticMesh : public Resource
	{
	public:
		/*
		* @brief Constructor.
		* @param aVertices The vertices of the mesh.
		* @param aIndices The indices of the mesh.
		*/
		StaticMesh(const std::vector<StaticVertex>& aVertices, const std::vector<uint32_t>& aIndices);
		
		/*
		* @brief Destructor.
		*/
		virtual ~StaticMesh();

		/*
		* @brief Returns the resource type.
		* @return The resource type.
		*/
		[[nodiscard]] static ResourceType GetStaticType() { return ResourceType::MESH; }

		/*
		* @copydoc Resource::GetType()
		*/
		[[nodiscard]] const ResourceType GetType() const { return GetStaticType(); }

		/*
		* @brief Returns the vertex buffer.
		* @return Instance of the vertex buffer.
		*/
		[[nodiscard]] const std::shared_ptr<IVertexBuffer>& GetVertexBuffer() const { return mVertexBuffer; }
		
		/*
		* @brief Returns the index buffer.
		* @return Instance of the index buffer.
		*/
		[[nodiscard]] const std::shared_ptr<IIndexBuffer>& GetIndexBuffer() const { return mIndexBuffer; }

	private:
		std::vector<StaticVertex> mVertices; // Container of the vertices.
		std::vector<uint32_t> indices; // Container of the indices.

		std::shared_ptr<IVertexBuffer> mVertexBuffer; // Instance of the vertex buffer.
		std::shared_ptr<IIndexBuffer> mIndexBuffer; // Instance of the index buffer.
	};
}