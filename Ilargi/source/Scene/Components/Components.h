#pragma once

#include <glm.hpp>
#include <string>

namespace Ilargi
{
	class VertexBuffer;
	class IndexBuffer;

	struct TransformComponent
	{
		glm::mat4 transform;
	};

	struct InfoComponent
	{
		std::string name;
	};

	struct MeshComponent
	{
		std::shared_ptr<VertexBuffer> vertexBuffer;
		std::shared_ptr<IndexBuffer> indexBuffer;
	};
}