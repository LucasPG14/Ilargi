#pragma once

#include <entt.hpp>

#include "Components/Components.h"

namespace Ilargi
{
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