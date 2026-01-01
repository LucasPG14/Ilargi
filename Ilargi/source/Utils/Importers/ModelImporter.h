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
		uint32_t magic{ 0x4D4F444CU };
		uint32_t version{ 1U };
		uint32_t childrens{ 0U };
	};

	struct MaterialHeader
	{
		uint32_t magic{ 0x4D41544CU };
		uint32_t version{ 1U };
	};

	struct MeshHeader
	{
		uint32_t magic{ 0x4D455348U };
		uint32_t version{ 1U };
		uint32_t verticesCount{ 0U };
		uint32_t indicesCount{ 0U };
 	};

	struct ModelNode;

	class ModelImporter
	{
	public:
		/*
		* @brief Imports a model.
		* @param aUUID The UUID of the resource.
		* @param aMetadata The metadata of the model.
		*/
		static void ImportModel(UUID aUUID, const ResourceMetadata& aMetadata);
		
		/*
		* @brief Loads a model.
		* @param aMetadata The metadata of the model.
		* @return An instance of the model loaded.
		*/
		static std::shared_ptr<Resource> LoadModel(const ResourceMetadata& aMetadata);
		
		/*
		* @brief Loads a mesh.
		* @param aMetadata The metadata of the mesh.
		* @return An instance of the mesh loaded.
		*/
		static std::shared_ptr<Resource> LoadMesh(const ResourceMetadata& aMetadata);
	private:
		/*
		* @brief Imports a mesh.
		* @param aMesh The assimp mesh.
		* @param aMetadata The metadata of the mesh.
		* @return The identifier of the mesh loaded.
		*/
		static UUID ImportMesh(const aiMesh* aMesh, const ResourceMetadata& aMetadata);
		
		/*
		* @brief Imports a material.
		* @param aMaterial The assimp material.
		* @param aMetadata The metadata of the material.
		* @return The identifier of the material loaded.
		*/
		static UUID ImportMaterial(const aiMaterial* aMaterial, const ResourceMetadata& aMetadata);
		
		/*
		* @brief Process the hierarchy of the model.
		* @param aScene The assimp scene.
		* @param aNode The assimp node to process.
		* @param aMeshesUUIDs Container with all the meshes of the model.
		* @param aMaterialsUUIDs Container with all the materials of the model.
		* @param aModelNodes Container where the nodes processed will be saved.
		* @return The identifier of the node.
		*/
		static uint32_t ProcessNode(const aiScene* aScene, const aiNode* aNode, const std::vector<UUID>& aMeshesUUIDs, const std::vector<UUID>& aMaterialsUUIDs, std::vector<ModelNode>& aModelNodes);
	};
}