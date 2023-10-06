#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/quaternion.hpp>
#include <string>

namespace Ilargi
{
	class VertexBuffer;
	class IndexBuffer;

	struct TransformComponent
	{
		glm::mat4 transform = glm::mat4(1.0f);
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);

		void CalculateTransform()
		{
			const glm::mat4& rotationMat = glm::toMat4(glm::quat(glm::radians(rotation)));

			transform = glm::translate(glm::mat4(1.0f), position) * rotationMat * glm::scale(glm::mat4(1.0f), scale);
		}
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

	struct DirectionalLightComponent
	{
		glm::vec3 radiance = glm::vec3(1.0f);
	};

	struct PointLightComponent
	{
		glm::vec3 radiance = glm::vec3(1.0f);
		float radius = 1.0f;
	};
}