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

		entt::registry& GetWorld() { return world; }

	private:
		entt::registry world;
	};
}