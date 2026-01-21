#pragma once

#include <glm.hpp>

namespace Ilargi
{
	bool DecomposeMatrix(const glm::mat4& aMatrix, glm::vec3& aPosition, glm::vec3& aRotation, glm::vec3& aScale);
}