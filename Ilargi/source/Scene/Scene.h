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

		const entt::registry& GetWorld() const { return world; }
		entt::registry& GetWorld() { return world; }

	private:
		entt::registry world;
	};
}