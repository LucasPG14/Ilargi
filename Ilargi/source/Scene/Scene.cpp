#include "ilargipch.h"

#include "Scene.h"

namespace Ilargi
{
	Scene::Scene()
	{
		entt::entity entity = world.create();
		world.emplace<TransformComponent>(entity, glm::mat4(1.0f));
		world.emplace<InfoComponent>(entity, "Directional Light");
		entity = world.create();
		world.emplace<TransformComponent>(entity, glm::mat4(1.0f));
		world.emplace<InfoComponent>(entity, "My Entity 2");
		entity = world.create();
		world.emplace<TransformComponent>(entity, glm::mat4(1.0f));
		world.emplace<InfoComponent>(entity, "My Entity 3");
	}
	
	Scene::~Scene()
	{
	}
}