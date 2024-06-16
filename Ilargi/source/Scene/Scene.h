#pragma once

#include <entt.hpp>

#include "Components/Components.h"

namespace Ilargi
{
	using Entity = entt::entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void Destroy();

		Entity CreateEntity(const std::string& name = "Entity");
		void DestroyEntity(Entity entity);

		template<typename T, typename... Args>
		void CreateComponent(Entity entity, Args&& ...args)
		{
			world.emplace<T>(entity, std::forward<Args>(args)...);
		}

		const entt::registry& GetWorld() const { return world; }
		entt::registry& GetWorld() { return world; }

	private:
		entt::registry world;
	};
}