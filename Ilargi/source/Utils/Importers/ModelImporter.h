#pragma once

#include "Base/UUID.h"
#include "Resources/Resource.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

namespace Ilargi
{
	struct ModelHeader
	{
		uint32_t magic = 0x4D4F444C;
		uint32_t version = 1;
		uint32_t childrens;
	};

	struct MaterialHeader
	{
		uint32_t magic = 0x4D41544C;
		uint32_t version = 1;
	};

	struct MeshHeader
	{
		uint32_t magic = 0x4D455348;
		uint32_t version = 1;
		uint32_t verticesCount;
		uint32_t indicesCount;
 	};

	struct ModelNode;

	class ModelImporter
	{
	public:
		static void ImportModel(UUID aUUID, const ResourceMetadata& aMetadata);
		static std::shared_ptr<Resource> LoadModel(const ResourceMetadata& aMetadata);
		static std::shared_ptr<Resource> LoadMesh(const ResourceMetadata& aMetadata);
	private:
		static UUID ImportMesh(const aiMesh* aMesh, const ResourceMetadata& aMetadata);
		static UUID ImportMaterial(const aiMaterial* aMaterial, const ResourceMetadata& aMetadata);
		
		static uint32_t ProcessNode(const aiScene* aScene, const aiNode* aNode, const std::vector<UUID>& aMeshesUUIDs, const std::vector<UUID>& aMaterialsUUIDs, std::vector<ModelNode>& aModelNodes);
	};
}