#include "ilargipch.h"

#include "Scene.h"
#include "Renderer/Renderer.h"
#include "Renderer/UniformBuffer.h"

#include "Resources/Model.h"

#include "Utils/Importers/ModelImporter.h"

namespace Ilargi
{
	Scene::Scene()
	{
		mSceneDataUBO = UniformBuffer::Create(sizeof(SceneData), Renderer::GetConfig().maxFrames);
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

		CreateComponent<TransformComponent>(entity, glm::mat4(1.0f));
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
	
	void Scene::UpdatePointLights(glm::mat4 aMatrix, glm::vec3 aPosition)
	{
		mSceneData.viewProjMatrix = aMatrix;
		mSceneData.cameraPosition = aPosition;

		const auto& view = mWorld.view<TransformComponent, PointLightComponent>();
		mSceneData.pointLightsSize = 0;

		for (auto entity : view)
		{
			const auto&& [transform, light] = view.get<>(entity);

			PointLightUniformBuffer pointLight;
			pointLight.radiance = light.radiance;
			pointLight.radius = light.radius;
			pointLight.position = transform.position;

			mSceneData.pointLights[mSceneData.pointLightsSize++] = pointLight;
		}

		mSceneDataUBO->SetData(&mSceneData);
	}
}