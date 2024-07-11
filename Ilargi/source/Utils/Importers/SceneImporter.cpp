#include "ilargipch.h"

#include "SceneImporter.h"
#include "Scene/Scene.h"

#include "Resources/Mesh.h"

#include <ArduinoJson-v7.0.4.h>

namespace Ilargi
{
	void SceneImporter::ImportScene(UUID uuid, const ResourceMetadata& metadata)
	{
		if (metadata.sourceFile != metadata.filepath)
			std::filesystem::copy(metadata.sourceFile, metadata.filepath);
	}

	std::shared_ptr<Resource> SceneImporter::LoadScene(const ResourceMetadata& metadata)
	{
		std::shared_ptr<Scene> scene = std::make_shared<Scene>();

		JsonDocument document;

		std::ifstream file(metadata.filepath, std::ios::in);

		deserializeJson(document, file);

		for (int index = 0; index < document.size(); ++index)
		{
			const auto& node = document[index];
			const Entity entity = scene->CreateEntity(node["InfoComponent"]["Name"]);

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

			if (document[index].containsKey("StaticMeshComponent"))
			{
				const StaticMeshComponent& staticMesh = scene->CreateComponent<StaticMeshComponent>(entity);

				staticMesh.staticMesh->resourceUUID = static_cast<uint64_t>(document[index]["StaticMeshComponent"]["UUID"]);
			}
		}

		return scene;
	}
	
	void SceneImporter::SaveScene(std::shared_ptr<Scene> scene, const std::filesystem::path& path)
	{
		JsonDocument document;

		auto& world = scene->GetWorld();
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

			if (world.try_get<StaticMeshComponent>(entity))
			{
				const StaticMeshComponent& staticMesh = world.get<StaticMeshComponent>(entity);

				document[index]["StaticMeshComponent"]["UUID"] = static_cast<uint64_t>(staticMesh.staticMesh->resourceUUID);
			}
		}

		std::ofstream file(path, std::ios::out);

		serializeJsonPretty(document, file);

		file.close();
	}
}