#include "ilargipch.h"

#include "Scene.h"
#include "Renderer/Renderer.h"
#include "Renderer/UniformBuffer.h"

#include "Resources/Model.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Resources/ResourceManager.h"

#include "Utils/Importers/ModelImporter.h"

namespace Ilargi
{
	Scene::Scene()
	{
		mSceneDataUBO = UniformBuffer::Create(sizeof(SceneData), Renderer::GetConfig().maxFrames);
	}
	
	Scene::~Scene()
	{
		mWorld.clear();
	}
	
	void Scene::Destroy()
	{
		auto meshStorage{ mWorld.view<StaticMeshComponent>() };
		for (auto entity : meshStorage)
		{
			mWorld.destroy(entity);
		}
		mWorld.clear();

		mSceneDataUBO->Destroy();
	}

	void Scene::LoadModel(const std::shared_ptr<Model>& model)
	{
		// TODO: Refactor this
		const std::vector<ModelNode>& modelNodes{ model->GetModelNodes() };

		std::vector<Entity> entities;
		for (uint32_t index{ 0U }; index < modelNodes.size(); ++index)
		{
			ModelNode modelNode{ modelNodes[index] };
			entities.push_back(CreateEntity(modelNode.name));
			if (modelNode.mesh != UINT64_MAX)
			{
				std::shared_ptr<Material> material{ modelNode.material != UINT64_MAX ? std::static_pointer_cast<Material>(ResourceManager::GetResource(modelNode.material)) : Renderer::GetDefaultMaterial() };
				CreateComponent<StaticMeshComponent>(entities[index], std::static_pointer_cast<StaticMesh>(ResourceManager::GetResource(modelNode.mesh)), material);
			}
		}

		for (uint32_t index{ 0U }; index < modelNodes.size(); ++index)
		{
			const ModelNode& modelNode{ modelNodes[index] };
			Entity& entity{ entities[index] };
			FamilyComponent& familyComponent{ mWorld.get<FamilyComponent>(entity) };

			for (uint32_t childrenIndex{ 0U }; childrenIndex < modelNode.childrens.size(); ++childrenIndex)
			{
				Entity& childrenEntity{ entities[modelNode.childrens[childrenIndex]] };
				familyComponent.children.push_back(childrenEntity);

				FamilyComponent& familyChildrenComponent{ mWorld.get<FamilyComponent>(childrenEntity) };
				familyChildrenComponent.parent = entity;
			}
		}
	}
	
	Entity Scene::CreateEntity(const std::string& aName, const glm::mat4& aTransform)
	{
		Entity entity{ mWorld.create() };

		CreateComponent<TransformComponent>(entity, aTransform);
		CreateComponent<InfoComponent>(entity, aName.c_str());
		CreateComponent<FamilyComponent>(entity);

		return entity;
	}

	Entity Scene::CreateChildrenEntity(Entity aEntity, const std::string& aName, const glm::mat4& aTransform)
	{
		Entity childEntity{ CreateEntity(aName, aTransform) };

		auto& family{ mWorld.get<FamilyComponent>(aEntity) };
		family.children.push_back(childEntity);

		auto& familyChildren{ mWorld.get<FamilyComponent>(childEntity) };
		familyChildren.parent = aEntity;

		return childEntity;
	}

	void Scene::DestroyEntity(Entity aEntity)
	{
		const auto& parentEntity{ mWorld.get<FamilyComponent>(aEntity).parent };
		if (parentEntity != entt::null)
		{
			auto& childrens{ mWorld.get<FamilyComponent>(parentEntity).children };
			std::remove(childrens.begin(), childrens.end(), aEntity);
		}
		mWorld.destroy(aEntity);
	}
	
	void Scene::UpdatePointLights(glm::mat4 aMatrix, glm::vec3 aPosition)
	{
		mSceneData.viewProjMatrix = aMatrix;
		mSceneData.cameraPosition = aPosition;

		const auto& view{ mWorld.view<TransformComponent, PointLightComponent>() };
		mSceneData.pointLightsSize = 0;

		for (auto entity : view)
		{
			const auto&& [transform, light] { view.get<>(entity)};

			PointLightUniformBuffer pointLight;
			pointLight.radiance = light.radiance;
			pointLight.radius = light.radius;
			pointLight.position = transform.position;

			mSceneData.pointLights[mSceneData.pointLightsSize++] = pointLight;
		}

		mSceneDataUBO->SetData(&mSceneData);
	}
	
	void Scene::CalculateChildrenTransforms(Entity entity, const glm::mat4& aMatrix)
	{
		TransformComponent& transformComponent{ GetComponent<TransformComponent>(entity) };
		FamilyComponent& familyComponent{ GetComponent<FamilyComponent>(entity) };
		for (uint32_t index{ 0U }; index < familyComponent.children.size(); ++index)
		{
			Entity children{ familyComponent.children[index] };
			TransformComponent& childrenTransform{ GetComponent<TransformComponent>(children) };
			childrenTransform.CalculateWorldTransform(transformComponent.worldTransform);
			CalculateChildrenTransforms(children, childrenTransform.worldTransform);
		}
	}
}