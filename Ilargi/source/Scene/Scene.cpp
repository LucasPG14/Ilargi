#include "ilargipch.h"

#include "Scene.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"

#include "Utils/Importers/ModelImporter.h"

namespace Ilargi
{
	Scene::Scene()
	{
		std::shared_ptr<StaticMesh> mesh = ModelImporter::ImportModel("assets/models/viking_room2.obj");

		Entity entity = CreateEntity("Directional Light");
		world.emplace<DirectionalLightComponent>(entity);

		for (int i = 0; i < 2; ++i)
		{
			entity = CreateEntity("Model");
			world.emplace<StaticMeshComponent>(entity, mesh);
		}
	}
	
	Scene::~Scene()
	{
	}
	
	void Scene::Destroy()
	{
		auto meshStorage = world.view<StaticMeshComponent>();
		for (auto entity : meshStorage)
		{
			//auto mesh = meshStorage.get(entity);
			//mesh._Myfirst._Val.staticMesh->Destroy();
			//mesh._Myfirst._Val.vertexBuffer->Destroy();
			world.destroy(entity);
		}
		world.clear();
	}
	
	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = world.create();
		world.emplace<TransformComponent>(entity, mat4(1.0f));
		world.emplace<InfoComponent>(entity, name.c_str());

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		world.destroy(entity);
	}
}