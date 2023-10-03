#pragma once

#include <glm.hpp>
#include <string>

namespace Ilargi
{
	struct TransformComponent
	{
		glm::mat4 transform;
	};

	struct InfoComponent
	{
		std::string name;
	};
}