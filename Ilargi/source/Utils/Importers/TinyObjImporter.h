#pragma once

#include <glm.hpp>

namespace Ilargi
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 texCoord;
	};

	void ImportModel(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
}