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
		glm::vec3 radiance;
		float radius;
		alignas(16)glm::vec3 position;
	};

	struct SceneData
	{
		glm::mat4 viewProjMatrix {};
		glm::vec3 cameraPosition {};
		uint32_t pointLightsSize{ 0U };
		std::array<PointLightUniformBuffer, 1024> pointLights;
	};

	class Scene : public Resource
	{
	public:
		Scene();
		~Scene();

		static ResourceType GetStaticType() { return ResourceType::SCENE; }
		const ResourceType GetType() const { return GetStaticType(); }

		void Destroy();

		void LoadModel(const std::shared_ptr<Model>& model);

		Entity CreateEntity(const std::string& aName = "Entity", const glm::mat4& aTransform = glm::mat4(1.0));
		Entity CreateChildrenEntity(Entity aEntity, const std::string& aName = "Entity", const glm::mat4& aTransform = glm::mat4(1.0));
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

		template<typename T>
		T& GetComponent(Entity aEntity)
		{
			ILG_ASSERT(HasComponent<T>(aEntity), "This entity doesn't have this component");
			return mWorld.get<T>(aEntity);
		}

		void CalculateChildrenTransforms(Entity entity, const glm::mat4& aMatrix);

		[[nodiscard]] const entt::registry& GetWorld() const { return mWorld; }
		[[nodiscard]] entt::registry& GetWorld() { return mWorld; }

		[[nodiscard]] const std::shared_ptr<UniformBuffer> GetSceneDataUBO() const { return mSceneDataUBO; }

	private:
		entt::registry mWorld;

		std::shared_ptr<UniformBuffer> mSceneDataUBO;
		SceneData mSceneData;
	};
}