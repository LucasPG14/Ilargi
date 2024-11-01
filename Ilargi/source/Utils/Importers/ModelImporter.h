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

	struct MeshInfo
	{
		uint32_t vertices;
		uint32_t indices;
		uint32_t materialIndex;
	};

	struct MaterialInfo
	{
		vec4 color;
	};

	class StaticMesh;
	class Scene;

	class ModelImporter
	{
	public:
		static void ImportModel(UUID aUUID, const ResourceMetadata& aMetadata);
		static void ImportModel2(UUID aUUID, const ResourceMetadata& aMetadata);
		static std::shared_ptr<Resource> LoadModel(const ResourceMetadata& aMetadata);
		static std::shared_ptr<Resource> LoadModel2(const ResourceMetadata& aMetadata);
		
		static void ImportFBX(const std::filesystem::path& aFilepath, const std::shared_ptr<Scene>& aScene);
	};
}