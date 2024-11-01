#include "ilargipch.h"

#include "Scene.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"

#include "Resources/Model.h"

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
		auto meshStorage = mWorld.view<StaticMeshComponent>();
		for (auto entity : meshStorage)
		{
			mWorld.destroy(entity);
		}
		mWorld.clear();
	}

	void Scene::LoadModel(const std::shared_ptr<Model>& model)
	{
		// TODO: Refactor this
		const std::vector<std::shared_ptr<StaticMesh>> meshes = model->GetMeshes();
		const std::vector<std::shared_ptr<Material>> materials = model->GetMaterials();

		for (int i = 0; i < meshes.size(); ++i)
		{
			Entity entity = CreateEntity();
			CreateComponent<StaticMeshComponent>(entity, meshes[i], materials[i]);
		}
	}
	
	Entity Scene::CreateEntity(const std::string& aName)
	{
		Entity entity = mWorld.create();

		CreateComponent<TransformComponent>(entity, mat4(1.0f));
		CreateComponent<InfoComponent>(entity, aName.c_str());
		CreateComponent<FamilyComponent>(entity);

		return entity;
	}

	Entity Scene::CreateChildrenEntity(Entity aEntity, const std::string& aName)
	{
		Entity childEntity = CreateEntity(aName);

		auto& family = mWorld.get<FamilyComponent>(aEntity);
		family.children.push_back(childEntity);

		auto& familyChildren = mWorld.get<FamilyComponent>(childEntity);
		familyChildren.parent = aEntity;

		return childEntity;
	}

	void Scene::DestroyEntity(Entity aEntity)
	{
		mWorld.destroy(aEntity);
	}
}