#pragma once

#include <entt.hpp>

#include "Resources/Resource.h"
#include "Components/Components.h"

namespace Ilargi
{
	class Model;
	class UniformBuffer;

	struct PointLightUniformBuffer
	{
		vec4 radiance;
		vec3 position;
		float radius;
	};

	struct SceneLights
	{
		std::array<PointLightUniformBuffer, 1024> pointLights;
		uint32_t pointLightsSize = 0;
	};

	class Scene : public Resource
	{
	public:
		Scene();
		~Scene();

		const ResourceType GetType() const override { return ResourceType::SCENE; }

		void Destroy();

		void LoadModel(const std::shared_ptr<Model>& model);

		Entity CreateEntity(const std::string& aName = "Entity");
		Entity CreateChildrenEntity(Entity aEntity, const std::string& aName = "Entity");
		void DestroyEntity(Entity aEntity);

		void UpdatePointLights();

		template<typename T, typename... Args>
		T& CreateComponent(Entity aEntity, Args&& ...aArgs)
		{
			return mWorld.emplace<T>(aEntity, std::forward<Args>(aArgs)...);
		}

		template<typename T>
		bool HasComponent(Entity aEntity)
		{
			return mWorld.try_get<T>(aEntity);
		}

		template<typename T>
		void DestroyComponent(Entity aEntity)
		{
			ILG_ASSERT(HasComponent<T>(aEntity), "This entity doesn't have this component");
			mWorld.remove<T>(aEntity);
		}

		const entt::registry& GetWorld() const { return mWorld; }
		entt::registry& GetWorld() { return mWorld; }

		const std::shared_ptr<UniformBuffer> GetPointLightsUBO() const { return mPointLightsUBO; }

	private:
		entt::registry mWorld;

		std::shared_ptr<UniformBuffer> mPointLightsUBO;
		SceneLights mSceneLights;
	};
}