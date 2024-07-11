#pragma once

#include <entt.hpp>

#include "Resources/Resource.h"
#include "Components/Components.h"

namespace Ilargi
{
	class Scene : public Resource
	{
	public:
		Scene();
		~Scene();

		const ResourceType GetType() const override { return ResourceType::SCENE; }

		void Destroy();

		Entity CreateEntity(const std::string& name = "Entity");
		Entity CreateChildrenEntity(Entity entity, const std::string& name = "Entity");
		void DestroyEntity(Entity entity);

		template<typename T, typename... Args>
		T& CreateComponent(Entity entity, Args&& ...args)
		{
			return world.emplace<T>(entity, std::forward<Args>(args)...);
		}

		const entt::registry& GetWorld() const { return world; }
		entt::registry& GetWorld() { return world; }

	private:
		entt::registry world;
	};
}