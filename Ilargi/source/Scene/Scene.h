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
		glm::vec4 radiance;
		glm::vec3 position;
		float radius;
	};

	struct SceneData
	{
		glm::mat4 viewProjMatrix;
		glm::vec3 cameraPosition;
		uint32_t pointLightsSize = 0;
		std::array<PointLightUniformBuffer, 1024> pointLights;
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

		void UpdatePointLights(glm::mat4 aMatrix, glm::vec3 aPosition);

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

		const std::shared_ptr<UniformBuffer> GetPointLightsUBO() const { return mSceneDataUBO; }

	private:
		entt::registry mWorld;

		std::shared_ptr<UniformBuffer> mSceneDataUBO;
		SceneData mSceneData;
	};
}