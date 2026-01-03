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
		glm::mat4 projMatrix {};
		glm::mat4 viewMatrix {};
		glm::vec3 cameraPosition {};
		uint32_t pointLightsSize{ 0U };
		std::array<PointLightUniformBuffer, 1024> pointLights;
	};

	class Scene : public Resource
	{
	public:
		/*
		* @brief Constructor.
		*/
		Scene();

		/*
		* @brief Destructor.
		*/
		~Scene();

		/*
		* @brief Updates the entities.
		*/
		void Update();

		/*
		* @brief Returns the resource type.
		* @return The resource type.
		*/
		static ResourceType GetStaticType() { return ResourceType::SCENE; }
		
		/*
		* @copydoc Resource::GetType().
		*/
		const ResourceType GetType() const { return GetStaticType(); }

		/*
		* @brief Destroys the scene data.
		*/
		void Destroy();

		/*
		* @brief Loads the given model into the scene.
		* @param aModel The model to add in the scene.
		*/
		void LoadModel(const std::shared_ptr<Model>& aModel);

		/*
		* @brief Creates an entity and return its identifier.
		* @param aName The name of the entity. By default is "Entity".
		* @param aTransform The local transform of the entity. By default is glm::mat4(1.0).
		* @param aEntityId The identifier of the entity to create. By default is null.
		* @return The entity identifier.
		*/
		Entity CreateEntity(const std::string& aName = "Entity", const glm::mat4& aTransform = glm::mat4(1.0), const entt::entity aEntityId = entt::null);
		
		/*
		* @brief Creates an entity child and return its identifier.
		* @param aEntity The parent of the entity.
		* @param aName The name of the entity. By default is "Entity".
		* @param aTransform The local transform of the entity. By default is glm::mat4(1.0).
		* @return The entity identifier.
		*/
		Entity CreateChildrenEntity(Entity aEntity, const std::string& aName = "Entity", const glm::mat4& aTransform = glm::mat4(1.0));
		
		/*
		* @brief Destroys the specified entity.
		* @param aEntity The entity to destroy.
		*/
		void DestroyEntity(Entity aEntity);

		/*
		* @brief Updates the point lights.
		* @param aProj The projection matrix.
		* @param aView The view matrix.
		* @param aPosition The camera position.
		*/
		void UpdatePointLights(const glm::mat4 aProj, const glm::mat4 aView, const glm::vec3 aPosition);

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
		[[nodiscard]] T& GetComponent(Entity aEntity)
		{
			ILG_ASSERT(HasComponent<T>(aEntity), "This entity doesn't have this component");
			return mWorld.get<T>(aEntity);
		}

		template<typename T>
		[[nodiscard]] T& GetOrCreateComponent(Entity aEntity)
		{
			return mWorld.get_or_emplace<T>(aEntity);
		}

		/*
		* @brief Calculates the children transforms.
		* @param aEntity The parent entity.
		* @param aMatrix The parent world transform.
		*/
		void CalculateChildrenTransforms(Entity entity, const glm::mat4& aMatrix);

		/*
		* @brief Returns the world of entities.
		* @return The world of entities.
		*/
		[[nodiscard]] const entt::registry& GetWorld() const { return mWorld; }
		
		/*
		* @brief Returns the world of entities.
		* @return The world of entities.
		*/
		[[nodiscard]] entt::registry& GetWorld() { return mWorld; }

		/*
		* @brief Returns the uniform buffer of the scene data.
		* @return The scene data uniform buffer.
		*/
		[[nodiscard]] const std::shared_ptr<UniformBuffer> GetSceneDataUBO() const { return mSceneDataUBO; }

	private:
		SceneData mSceneData; // Instance of the scene data.
		entt::registry mWorld; // Instance of the entity world.

		std::shared_ptr<UniformBuffer> mSceneDataUBO; // Instance of the scene data uniform buffer.
	};
}