#pragma once

#include "Resource.h"

#include <vec2.hpp>
#include <vec3.hpp>

namespace Ilargi
{
	struct StaticVertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
		glm::vec3 tangents;
		glm::vec3 bitangents;
	};

	class StaticMesh : public Resource
	{
	public:
		StaticMesh(const std::vector<StaticVertex>& vertices, const std::vector<uint32_t>& indices);
		virtual ~StaticMesh();
	};
}