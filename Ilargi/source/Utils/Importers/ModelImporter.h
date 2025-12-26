#pragma once

#include "Base/UUID.h"
#include "Resources/Resource.h"

struct aiScene;
struct aiNode;

namespace Ilargi
{
	struct ComponentNode
	{
	};

	struct MeshNode : public ComponentNode
	{
		uint32_t meshID;
		uint32_t materialID;
 	};

	struct EntityNode
	{
		std::string name;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
		MeshNode meshNode;
		int numChildren{ -1 };
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
		glm::vec4 color;
	};

	class StaticMesh;
	class Scene;

	class ModelImporter
	{
	public:
		static void ImportModel(UUID aUUID, const ResourceMetadata& aMetadata);
		static void ImportModel2(UUID aUUID, const ResourceMetadata& aMetadata);
		static std::shared_ptr<Resource> LoadModel(const ResourceMetadata& aMetadata);

	private:
		static void ReturnModelHierarchy(const aiScene* aScene, const aiNode* aNode, std::vector<EntityNode>& hierarchy);
	};
}