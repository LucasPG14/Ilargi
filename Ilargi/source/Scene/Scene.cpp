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
		std::shared_ptr<StaticMesh> mesh = ModelImporter::ImportModel("models/viking_room2.obj");

		for (int i = 0; i < 1; ++i)
		{
			Entity entity = world.create();
			world.emplace<TransformComponent>(entity, glm::mat4(1.0f));
			world.emplace<InfoComponent>(entity, "Model");
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
			//world.destroy(entity);
		}
		world.clear();
	}
}