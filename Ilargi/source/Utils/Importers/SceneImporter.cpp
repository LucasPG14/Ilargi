#include "ilargipch.h"

#include "SceneImporter.h"
#include "Scene/Scene.h"

#include "Resources/ResourceManager.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"

#include <ArduinoJson-v7.0.4.h>

namespace Ilargi
{
	void SceneImporter::ImportScene(UUID aUUID, const ResourceMetadata& aMetadata)
	{
		if (aMetadata.sourceFile != aMetadata.filepath)
			std::filesystem::copy(aMetadata.sourceFile, aMetadata.filepath);
	}

	std::shared_ptr<Resource> SceneImporter::LoadScene(const ResourceMetadata& aMetadata)
	{
		std::shared_ptr<Scene> scene = std::make_shared<Scene>();

		JsonDocument document;

		std::ifstream file(aMetadata.filepath, std::ios::in);

		deserializeJson(document, file);

		for (int index = 0; index < document.size(); ++index)
		{
			const auto& node = document[index];
			const Entity entity = scene->CreateEntity(node["InfoComponent"]["Name"]);

			// TODO: Create own function to store vectors
			auto& transform = scene->GetWorld().get<TransformComponent>(entity);
			transform.position.x = node["TransformComponent"]["Position"]["x"];
			transform.position.y = node["TransformComponent"]["Position"]["y"];
			transform.position.z = node["TransformComponent"]["Position"]["z"];

			transform.rotation.x = node["TransformComponent"]["Rotation"]["x"];
			transform.rotation.y = node["TransformComponent"]["Rotation"]["y"];
			transform.rotation.z = node["TransformComponent"]["Rotation"]["z"];

			transform.scale.x = node["TransformComponent"]["Scale"]["x"];
			transform.scale.y = node["TransformComponent"]["Scale"]["y"];
			transform.scale.z = node["TransformComponent"]["Scale"]["z"];

			if (node.containsKey("DirectionalLightComponent"))
			{
				DirectionalLightComponent& dirLight = scene->CreateComponent<DirectionalLightComponent>(entity);

				dirLight.radiance.x = node["DirectionalLightComponent"]["Radiance"]["x"];
				dirLight.radiance.y = node["DirectionalLightComponent"]["Radiance"]["y"];
				dirLight.radiance.z = node["DirectionalLightComponent"]["Radiance"]["z"];
				dirLight.radiance.w = node["DirectionalLightComponent"]["Radiance"]["w"];
			}

			if (node.containsKey("StaticMeshComponent"))
			{
				StaticMeshComponent& staticMesh = scene->CreateComponent<StaticMeshComponent>(entity);

				UUID uuid = static_cast<uint64_t>(node["StaticMeshComponent"]["UUID"]);
				
				staticMesh.staticMesh = std::static_pointer_cast<StaticMesh>(ResourceManager::GetResource(uuid));
			}
		}

		return scene;
	}
	
	void SceneImporter::SaveScene(const std::shared_ptr<Scene>& aScene, const std::filesystem::path& aFilepath)
	{
		JsonDocument document;

		auto& world = aScene->GetWorld();
		auto& entities = world.storage<Entity>();

		for (auto& entity : entities)
		{
			uint64_t index = static_cast<uint64_t>(entity);

			auto s = document[index]["TransformComponent"];

			const auto& transform = world.get<TransformComponent>(entity);
			document[index]["TransformComponent"]["Position"]["x"] = transform.position.x;
			document[index]["TransformComponent"]["Position"]["y"] = transform.position.y;
			document[index]["TransformComponent"]["Position"]["z"] = transform.position.z;

			document[index]["TransformComponent"]["Rotation"]["x"] = transform.rotation.x;
			document[index]["TransformComponent"]["Rotation"]["y"] = transform.rotation.y;
			document[index]["TransformComponent"]["Rotation"]["z"] = transform.rotation.z;

			document[index]["TransformComponent"]["Scale"]["x"] = transform.scale.x;
			document[index]["TransformComponent"]["Scale"]["y"] = transform.scale.y;
			document[index]["TransformComponent"]["Scale"]["z"] = transform.scale.z;

			const auto& info = world.get<InfoComponent>(entity);
			document[index]["InfoComponent"]["Name"] = info.name;

			if (world.try_get<DirectionalLightComponent>(entity))
			{
				const DirectionalLightComponent& dirLight = world.get<DirectionalLightComponent>(entity);

				document[index]["DirectionalLightComponent"]["Radiance"]["r"] = dirLight.radiance.x;
				document[index]["DirectionalLightComponent"]["Radiance"]["g"] = dirLight.radiance.y;
				document[index]["DirectionalLightComponent"]["Radiance"]["b"] = dirLight.radiance.z;
				document[index]["DirectionalLightComponent"]["Radiance"]["a"] = dirLight.radiance.w;
			}

			if (world.try_get<StaticMeshComponent>(entity))
			{
				const StaticMeshComponent& staticMesh = world.get<StaticMeshComponent>(entity);

				UUID uuid = 0;

				if (auto mesh = staticMesh.staticMesh.lock())
				{
					uuid = mesh->mResourceUUID;
				}

				document[index]["StaticMeshComponent"]["UUID"] = static_cast<uint64_t>(uuid);

				if (auto material = staticMesh.material.lock())
				{
					document[index]["StaticMeshComponent"]["Color"]["r"] = material->GetMaterialData().color.x;
					document[index]["StaticMeshComponent"]["Color"]["g"] = material->GetMaterialData().color.y;
					document[index]["StaticMeshComponent"]["Color"]["b"] = material->GetMaterialData().color.z;
					document[index]["StaticMeshComponent"]["Color"]["a"] = material->GetMaterialData().color.w;
					document[index]["StaticMeshComponent"]["Metallic"] = material->GetMaterialData().metallic;
					document[index]["StaticMeshComponent"]["Roughness"] = material->GetMaterialData().roughness;
				}
			}
		}

		std::ofstream file(aFilepath, std::ios::out);

		serializeJson(document, file);

		file.close();
	}
}