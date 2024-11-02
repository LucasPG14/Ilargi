#pragma once

#include <mat4x4.hpp>
#include <vec2.hpp>
#include <vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtc/matrix_transform.hpp>
#include <gtx/euler_angles.hpp>

#include <string>

namespace Ilargi
{
	class VertexBuffer;
	class IndexBuffer;
	class StaticMesh;
	class Material;

	using Entity = entt::entity;

	struct FamilyComponent
	{
		Entity parent = entt::null;
		std::vector<Entity> children;
	};

	struct TransformComponent
	{
		glm::mat4 transform = glm::mat4(1.0f);
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);

		void CalculateTransform()
		{
			transform = glm::translate(glm::mat4(1.0), position) * glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z));
			transform = glm::scale(transform, scale);
		}
	};

	struct InfoComponent
	{
		std::string name;
	};

	struct StaticMeshComponent
	{
		std::weak_ptr<StaticMesh> staticMesh;
		std::weak_ptr<Material> material;
	};

	struct DirectionalLightComponent
	{
		glm::vec4 radiance = glm::vec4(1.0f);
	};

	struct PointLightComponent
	{
		glm::vec4 radiance = glm::vec4(1.0f);
		float radius = 1.0f;
	};
}