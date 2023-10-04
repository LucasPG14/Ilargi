#include "ilargipch.h"

#include "Scene.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"

#include "Utils/Importers/TinyObjImporter.h"

namespace Ilargi
{
	Scene::Scene()
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		ImportModel(vertices, indices);

		entt::entity entity = world.create();
		world.emplace<TransformComponent>(entity, glm::mat4(1.0f));
		world.emplace<InfoComponent>(entity, "Directional Light");
		world.emplace<MeshComponent>(entity, VertexBuffer::Create((void*)vertices.data(), static_cast<uint32_t>(vertices.size() * sizeof(Vertex))),
			IndexBuffer::Create((void*)indices.data(), static_cast<uint32_t>(indices.size())));
	}
	
	Scene::~Scene()
	{
	}
}