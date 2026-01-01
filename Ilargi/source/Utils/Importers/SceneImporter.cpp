#include "ilargipch.h"

#include "SceneImporter.h"
#include "Scene/Scene.h"

#include "Resources/ResourceManager.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Utils/JsonConverters.h"

namespace Ilargi
{
	void SceneImporter::ImportScene(UUID aUUID, const ResourceMetadata& aMetadata)
	{
		if (aMetadata.sourceFile != aMetadata.filepath)
			std::filesystem::copy(aMetadata.sourceFile, aMetadata.filepath);
	}

	std::shared_ptr<Resource> SceneImporter::LoadScene(const ResourceMetadata& aMetadata)
	{
		std::shared_ptr<Scene> scene{ std::make_shared<Scene>() };

		JsonDocument document;

		std::ifstream file(aMetadata.filepath, std::ios::in);

		deserializeJson(document, file);

		for (uint32_t index { 0U }; index < document.size(); ++index)
		{
			const auto& node{ document[index] };

			const Entity entity{ scene->CreateEntity(node["InfoComponent"]["Name"], glm::mat4(1.0), static_cast<entt::entity>(node["EntityID"].as<uint64_t>()))};

			auto& transform{ scene->GetWorld().get<TransformComponent>(entity) };
			transform.position = node["TransformComponent"]["Position"];
			transform.rotation = node["TransformComponent"]["Rotation"];
			transform.scale = node["TransformComponent"]["Scale"];

			if (node.containsKey("DirectionalLightComponent"))
			{
				DirectionalLightComponent& dirLight{ scene->CreateComponent<DirectionalLightComponent>(entity) };
				dirLight.radiance = node["DirectionalLightComponent"]["Radiance"];
			}

			if (node.containsKey("PointLightComponent"))
			{
				PointLightComponent& pointLight{ scene->CreateComponent<PointLightComponent>(entity) };

				pointLight.radiance = node["PointLightComponent"]["Radiance"];
				pointLight.radius = node["PointLightComponent"]["Radius"];
			}

			if (node.containsKey("StaticMeshComponent"))
			{
				StaticMeshComponent& staticMesh{ scene->CreateComponent<StaticMeshComponent>(entity) };

				UUID meshUUID{ static_cast<uint64_t>(node["StaticMeshComponent"]["Mesh"]) };
				UUID materialUUID{ static_cast<uint64_t>(node["StaticMeshComponent"]["Material"]) };
				
				staticMesh.staticMesh = std::static_pointer_cast<StaticMesh>(ResourceManager::GetResource(meshUUID));
				staticMesh.material = std::static_pointer_cast<Material>(ResourceManager::GetResource(materialUUID));
			}

			if (node.containsKey("ParentComponent"))
			{
				ParentComponent& parentComponent{ scene->CreateComponent<ParentComponent>(entity) };
				parentComponent.parent = static_cast<entt::entity>(node["ParentComponent"]["Parent"].as<uint64_t>());
			}

			if (node.containsKey("ChildComponent"))
			{
				ChildComponent& childComponent{ scene->CreateComponent<ChildComponent>(entity) };
				JsonArray childrenList{ document[index]["ChildComponent"]["Childrens"] };
				for (const auto& child : childrenList)
				{
					childComponent.childrens.push_back(static_cast<entt::entity>(child.as<uint64_t>()));
				}
			}
		}

		return scene;
	}
	
	void SceneImporter::SaveScene(const std::shared_ptr<Scene>& aScene, const std::filesystem::path& aFilepath)
	{
		JsonDocument document;

		auto& world{ aScene->GetWorld() };
		auto& entities{ world.storage<Entity>() };

		for (auto& entity : entities)
		{
			uint64_t index{ static_cast<uint64_t>(entity) };

			document[index]["EntityID"] = index;
			const auto& transform{ world.get<TransformComponent>(entity) };
			document[index]["TransformComponent"]["Position"] = transform.position;
			document[index]["TransformComponent"]["Rotation"] = transform.rotation;
			document[index]["TransformComponent"]["Scale"] = transform.scale;

			const auto& info{ world.get<InfoComponent>(entity) };
			document[index]["InfoComponent"]["Name"] = info.name;

			if (world.try_get<DirectionalLightComponent>(entity))
			{
				const DirectionalLightComponent& dirLight{ world.get<DirectionalLightComponent>(entity) };
				document[index]["DirectionalLightComponent"]["Radiance"] = dirLight.radiance;
			}

			if (world.try_get<PointLightComponent>(entity))
			{
				const PointLightComponent& pointLight{ world.get<PointLightComponent>(entity) };

				document[index]["PointLightComponent"]["Radiance"] = pointLight.radiance;
				document[index]["PointLightComponent"]["Radius"] = pointLight.radius;
			}

			if (world.try_get<StaticMeshComponent>(entity))
			{
				const StaticMeshComponent& staticMesh{ world.get<StaticMeshComponent>(entity) };

				UUID uuid{ 0U };

				if (auto mesh{ staticMesh.staticMesh.lock() })
				{
					uuid = mesh->mResourceUUID;
					document[index]["StaticMeshComponent"]["Mesh"] = static_cast<uint64_t>(uuid);
				}

				if (auto material{ staticMesh.material.lock() })
				{
					uuid = material->mResourceUUID;
					document[index]["StaticMeshComponent"]["Material"] = static_cast<uint64_t>(uuid);
				}
			}

			if (world.try_get<ParentComponent>(entity))
			{
				const ParentComponent& parentComponent{ world.get<ParentComponent>(entity) };
				document[index]["ParentComponent"]["Parent"] = static_cast<uint64_t>(parentComponent.parent);
			}

			if (world.try_get<ChildComponent>(entity))
			{
				const ChildComponent& childComponent{ world.get<ChildComponent>(entity) };
				JsonArray childrenArray{ document[index]["ChildComponent"]["Childrens"].to<JsonArray>()};
				for (auto child : childComponent.childrens)
				{
					childrenArray.add(static_cast<uint64_t>(child));
				}
			}
		}

		std::ofstream file(aFilepath, std::ios::out);

		serializeJsonPretty(document, file);

		file.close();
	}
}