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

	struct ParentComponent
	{
		Entity parent{ entt::null }; // Identifier of the parent entity.
	};

	struct ChildComponent
	{
		std::vector<Entity> childrens; // Container of children entities identifiers.
	};

	struct TransformComponent
	{
		glm::mat4 localTransform{ 1.0f }; // Local transform of the entity.
		glm::mat4 worldTransform{ 1.0f }; // World transform of the entity.
		glm::vec3 position{ 0.0f }; // The position of the entity.
		glm::vec3 rotation{ 0.0f }; // The rotation of the entity.
		glm::vec3 scale{ 1.0f }; // The scale of the entity.

		/*
		* @brief Calculates the local transform of the entity.
		*/
		void CalculateTransform()
		{
			localTransform = glm::translate(glm::mat4(1.0), position) * glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z));
			localTransform = glm::scale(localTransform, scale);
		}

		/*
		* @brief Calculates the world transform of the entity.
		*/
		void CalculateWorldTransform(const glm::mat4& aMatrix)
		{
			worldTransform = aMatrix * localTransform;
		}
	};

	struct InfoComponent
	{
		std::string name; // Name of the entity.
	};

	struct StaticMeshComponent
	{
		std::weak_ptr<StaticMesh> staticMesh; // Instance of the static mesh.
		std::weak_ptr<Material> material; // Instance of the material.
	};

	struct DirectionalLightComponent
	{
		glm::vec3 radiance{ 1.0f, 1.0f, 1.0f }; // Color of the light
	};

	struct PointLightComponent
	{
		glm::vec3 radiance{ 1.0f, 1.0f, 1.0f }; // Color of the light.
		float radius{ 1.0f }; // Radius of the point light.
	};

	struct CameraComponent
	{
		float fov{ glm::radians(60.0f) };
		float aspectRatio{ 16.0f / 9.0f }; // Aspect ratio.
		float nearPlane{ 0.1f }; // The minimum distance of the camera.
		float farPlane{ 1000.0f }; // The maximum distance of the camera.
	};
}