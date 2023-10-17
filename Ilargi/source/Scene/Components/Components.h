#pragma once

#include <Utils/Math/Math.h>

#include <string>

namespace Ilargi
{
	class VertexBuffer;
	class IndexBuffer;
	class StaticMesh;

	struct TransformComponent
	{
		mat4 transform = mat4(1.0f);
		vec3 position = vec3(0.0f);
		vec3 rotation = vec3(0.0f);
		vec3 scale = vec3(1.0f);

		void CalculateTransform()
		{
			//const mat4& rotationMat = mat4(quat(radians(rotation)));

			//glm::mat4 m = glm::toMat4(glm::quat(glm::radians(glm::vec3(rotation.x, rotation.y, rotation.z))));

			transform = math::translate(position) * mat4(quat(radians(rotation))) * math::scale(scale);
		}
	};

	struct InfoComponent
	{
		std::string name;
	};

	struct StaticMeshComponent
	{
		std::shared_ptr<StaticMesh> staticMesh;
	};

	struct DirectionalLightComponent
	{
		vec3 radiance = vec3(1.0f);
	};

	struct PointLightComponent
	{
		vec3 radiance = vec3(1.0f);
		float radius = 1.0f;
	};
}