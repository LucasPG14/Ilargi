#pragma once

#include "Base/UUID.h"
#include "Resources/Resource.h"

namespace Ilargi
{
	struct ComponentNode
	{
	};

	struct TransformNode : public ComponentNode
	{
		vec3 position;
		vec3 rotation;
		vec3 scale;
	};

	struct MeshNode : public ComponentNode
	{
		uint64_t uuid;
	};

	struct EntityNode
	{
		std::vector<ComponentNode> components;
		std::vector<EntityNode> children;
	};

	struct MeshesInfo
	{
		uint32_t vertices;
		uint32_t indices;
	};

	class StaticMesh;
	class Scene;

	class ModelImporter
	{
	public:
		static void ImportModel(UUID uuid, const ResourceMetadata& metadata);
		static std::shared_ptr<Resource> LoadModel(const ResourceMetadata& metadata);
		
		static void ImportFBX(const std::filesystem::path& path, const std::shared_ptr<Scene>& scene);
	};
}