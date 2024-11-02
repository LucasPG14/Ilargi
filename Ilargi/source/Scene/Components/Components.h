#pragma once

#include <Utils/Math/Math.h>

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
		mat4 transform = mat4(1.0f);
		vec3 position = vec3(0.0f);
		vec3 rotation = vec3(0.0f);
		vec3 scale = vec3(1.0f);

		void CalculateTransform()
		{
			transform = math::translate(position) * mat4(quat(radians(rotation))) * math::scale(scale);
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
		vec4 radiance = vec4(1.0f);
	};

	struct PointLightComponent
	{
		vec4 radiance = vec4(1.0f);
		float radius = 1.0f;
	};
}