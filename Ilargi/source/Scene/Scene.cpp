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
		mCameraDataUBO = UniformBuffer::Create(sizeof(CameraData), Renderer::GetConfig().maxFrames);
	}
	
	Scene::~Scene()
	{
		mWorld.clear();
	}

	void Scene::Update()
	{
		const auto& transformView{ mWorld.view<TransformComponent>() };
		for (auto entity : transformView)
		{
			TransformComponent& transform{ transformView.get<TransformComponent>(entity) };
			transform.CalculateTransform();
			if (HasComponent<ParentComponent>(entity))
			{
				const TransformComponent& parentTransform{GetComponent<TransformComponent>(GetComponent<ParentComponent>(entity).parent)};
				transform.CalculateWorldTransform(parentTransform.worldTransform);
			}
			else
			{
				transform.CalculateWorldTransform(glm::mat4(1.0f));
			}
		}
	}
	
	void Scene::Destroy()
	{
		mWorld.clear();

		mSceneDataUBO->Destroy();
		mCameraDataUBO->Destroy();
	}

	void Scene::LoadModel(const std::shared_ptr<Model>& model)
	{
		// TODO: Refactor this
		const std::vector<ModelNode>& modelNodes{ model->GetModelNodes() };

		std::vector<Entity> entities;
		for (uint32_t index{ 0U }; index < modelNodes.size(); ++index)
		{
			ModelNode modelNode{ modelNodes[index] };
			entities.push_back(CreateEntity(modelNode.name, modelNode.localTransform));
			if (!modelNode.submeshes.empty())
			{
				CreateComponent<StaticMeshComponent>(entities[index], modelNode.submeshes);
			}
		}

		for (uint32_t index{ 0U }; index < modelNodes.size(); ++index)
		{
			const ModelNode& modelNode{ modelNodes[index] };
			Entity& entity{ entities[index] };
			ChildComponent& childComponent{ mWorld.get_or_emplace<ChildComponent>(entity) };

			for (uint32_t childrenIndex{ 0U }; childrenIndex < modelNode.childrens.size(); ++childrenIndex)
			{
				Entity& childrenEntity{ entities[modelNode.childrens[childrenIndex]] };
				childComponent.childrens.push_back(childrenEntity);

				ParentComponent& parentComponent{ mWorld.get_or_emplace<ParentComponent>(childrenEntity) };
				parentComponent.parent = entity;
			}
		}
	}
	
	Entity Scene::CreateEntity(const std::string& aName, const glm::mat4& aTransform, const entt::entity aEntityId)
	{
		Entity entity{ mWorld.create(aEntityId) };

		CreateComponent<TransformComponent>(entity, aTransform);
		CreateComponent<InfoComponent>(entity, aName.c_str());

		return entity;
	}

	Entity Scene::CreateChildrenEntity(Entity aEntity, const std::string& aName, const glm::mat4& aTransform)
	{
		Entity childEntity{ CreateEntity(aName, aTransform) };

		ChildComponent& childComponent{ GetOrCreateComponent<ChildComponent>(aEntity) };
		childComponent.childrens.push_back(childEntity);
		
		CreateComponent<ParentComponent>(childEntity, aEntity);

		return childEntity;
	}

	void Scene::DestroyEntity(Entity aEntity)
	{
		if (HasComponent<ChildComponent>(aEntity))
		{
			auto& childrens{ GetComponent<ChildComponent>(aEntity).childrens };
			for (const auto& children : childrens)
			{
				DestroyEntity(children);
			}
		}
		if (HasComponent<ParentComponent>(aEntity))
		{
			const Entity& parentEntity{ GetComponent<ParentComponent>(aEntity).parent };
			auto& childrens{ GetComponent<ChildComponent>(parentEntity).childrens };
			std::remove(childrens.begin(), childrens.end(), aEntity);
		}
		mWorld.destroy(aEntity);
	}
	
	void Scene::UpdatePointLights(const glm::mat4 aProj, const glm::mat4 aView, const glm::vec3 aPosition)
	{
		mSceneData.projMatrix = aProj;
		mSceneData.viewMatrix = aView;
		mSceneData.cameraPosition = aPosition;

		const auto& view{ mWorld.view<TransformComponent, PointLightComponent>() };
		mSceneData.pointLightsSize = 0;

		for (auto entity : view)
		{
			const auto&& [transform, light] { view.get<>(entity)};

			PointLightUniformBuffer& pointLight{ mSceneData.pointLights[mSceneData.pointLightsSize++] };
			pointLight.radiance = light.radiance;
			pointLight.radius = light.radius;
			pointLight.position = transform.position;
		}

		mSceneDataUBO->SetData(&mSceneData, 0);
	}
	
	void Scene::CalculateChildrenTransforms(Entity aEntity, const glm::mat4& aMatrix)
	{
		if (HasComponent<ChildComponent>(aEntity))
		{
			ChildComponent& childComponent{ GetComponent<ChildComponent>(aEntity) };
			for (Entity children : childComponent.childrens)
			{
				TransformComponent& childrenTransform{ GetComponent<TransformComponent>(children) };
				childrenTransform.CalculateWorldTransform(aMatrix);
				CalculateChildrenTransforms(children, childrenTransform.worldTransform);
			}
		}
	}
}