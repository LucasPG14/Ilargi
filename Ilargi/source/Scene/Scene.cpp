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

		CreateComponent<TransformComponent>(entity, mat4(1.0f));
		CreateComponent<InfoComponent>(entity, name.c_str());
		CreateComponent<FamilyComponent>(entity);

		return entity;
	}

	Entity Scene::CreateChildrenEntity(Entity entity, const std::string& name)
	{
		Entity childEntity = CreateEntity();

		auto& family = world.get<FamilyComponent>(entity);
		family.children.push_back(childEntity);

		auto& familyChildren = world.get<FamilyComponent>(childEntity);
		familyChildren.parent = entity;

		return childEntity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		world.destroy(entity);
	}
}