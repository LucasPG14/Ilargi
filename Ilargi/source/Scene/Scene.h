#pragma once

#include <entt.hpp>

#include "Resources/Resource.h"
#include "Components/Components.h"

namespace Ilargi
{
	class Model;

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

		template<typename T, typename... Args>
		T& CreateComponent(Entity aEntity, Args&& ...aArgs)
		{
			return mWorld.emplace<T>(aEntity, std::forward<Args>(aArgs)...);
		}

		const entt::registry& GetWorld() const { return mWorld; }
		entt::registry& GetWorld() { return mWorld; }

	private:
		entt::registry mWorld;
	};
}