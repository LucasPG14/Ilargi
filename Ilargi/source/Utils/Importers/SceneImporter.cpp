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
			const Entity entity{ scene->CreateEntity(node["InfoComponent"]["Name"]) };

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

				UUID uuid{ static_cast<uint64_t>(node["StaticMeshComponent"]["UUID"]) };
				
				staticMesh.staticMesh = std::static_pointer_cast<StaticMesh>(ResourceManager::GetResource(uuid));
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
				}

				document[index]["StaticMeshComponent"]["UUID"] = static_cast<uint64_t>(uuid);

				if (auto material{ staticMesh.material.lock() })
				{
					document[index]["StaticMeshComponent"]["Color"] = material->GetMaterialData().color;
					document[index]["StaticMeshComponent"]["Metallic"] = material->GetMaterialData().metallic;
					document[index]["StaticMeshComponent"]["Roughness"] = material->GetMaterialData().roughness;
				}
			}
		}

		std::ofstream file(aFilepath, std::ios::out);

		serializeJsonPretty(document, file);

		file.close();
	}
}