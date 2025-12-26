#include "ilargipch.h"

#include "ModelImporter.h"
#include "Utils/FileSystem.h"

#include "Scene/Scene.h"

#include "Renderer/Renderer.h"
#include "Resources/ResourceManager.h"
#include "Resources/Model.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 

namespace Ilargi
{
	void ModelImporter::ImportModel(UUID aUUID, const ResourceMetadata& aMetadata)
	{
		Assimp::Importer importer;

		const aiScene* importScene{ importer.ReadFile(aMetadata.sourceFile.string().c_str(), aiProcess_CalcTangentSpace | aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices | aiProcess_SortByPType) };

		if (!importScene)
		{
			ILG_CORE_ERROR("Couldn't import model: {0}", aMetadata.sourceFile.string());
			return;
		}

		std::string shaderName { importScene->HasAnimations() ? "" : "PBR_Static" };

		std::vector<UUID> materialsInfo;
		materialsInfo.reserve(importScene->mNumMaterials);

		for (uint32_t materialIndex { 0U }; materialIndex < importScene->mNumMaterials; ++materialIndex)
		{
			const aiMaterial* aiMaterial{ importScene->mMaterials[materialIndex] };

			aiString name;
			aiMaterial->Get(AI_MATKEY_NAME, name);
			std::filesystem::path filepath{ aMetadata.filepath.parent_path() / name.C_Str() += ".imat"};

			MaterialData materialData;
			aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, materialData.color);
			aiMaterial->Get(AI_MATKEY_SHININESS, materialData.metallic);

			ResourceMetadata materialMetadata;
			materialMetadata.type = ResourceType::MATERIAL;
			materialMetadata.sourceFile = aMetadata.sourceFile;
			materialMetadata.filepath = filepath;
			
			Buffer buffer;
			buffer.size = sizeof(uint32_t) + shaderName.length() + sizeof(materialData);
			buffer.data = new char[buffer.size];

			uint32_t shaderNameSize { (uint32_t)shaderName.length() };

			char* buf{ buffer.data };
			memcpy(buf, &shaderNameSize, sizeof(uint32_t));
			buf += sizeof(uint32_t);

			memcpy(buf, shaderName.data(), shaderNameSize * sizeof(char));
			buf += shaderNameSize * sizeof(char);

			memcpy(buf, &materialData, sizeof(materialData));
			buf += sizeof(materialData);

			bool hasDiffuseTexture{ false };
			memcpy(buf, &hasDiffuseTexture, sizeof(bool));
			buf += sizeof(bool);

			FileSystem::WriteBinaryFile(materialMetadata.filepath, buffer);

			materialMetadata.lastWriteTime = std::filesystem::last_write_time(materialMetadata.filepath);

			materialsInfo.push_back(ResourceManager::RegisterResource(materialMetadata));
		}

		std::vector<MeshInfo> meshesInfo;
		meshesInfo.reserve(importScene->mNumMeshes);

		std::vector<StaticVertex> vertices;
		std::vector<uint32_t> indices;

		for (uint32_t meshIndex { 0U }; meshIndex < importScene->mNumMeshes; ++meshIndex)
		{
			const aiMesh* aiMesh { importScene->mMeshes[meshIndex] };

			bool hasNormals { aiMesh->HasNormals() };
			bool hasTexCoords { aiMesh->HasTextureCoords(0) };
			bool hasTangentsAndBitangents { aiMesh->HasTangentsAndBitangents() };

			uint32_t verticesCount { aiMesh->mNumVertices };
			uint32_t numFaces { aiMesh->mNumFaces };
			uint32_t indicesCount { numFaces * 3 };

			vertices.reserve(vertices.size() + verticesCount);
			indices.reserve(indices.size() + indicesCount);

			for (uint32_t vertexIndex { 0U }; vertexIndex < verticesCount; ++vertexIndex)
			{
				StaticVertex& vertex{ vertices.emplace_back() };
				vertex.position = { aiMesh->mVertices[vertexIndex].x, aiMesh->mVertices[vertexIndex].y, aiMesh->mVertices[vertexIndex].z };

				if (hasNormals)
					vertex.normal = { aiMesh->mNormals[vertexIndex].x, aiMesh->mNormals[vertexIndex].y, aiMesh->mNormals[vertexIndex].z };

				if (hasTexCoords)
					vertex.texCoord = { aiMesh->mTextureCoords[0][vertexIndex].x, aiMesh->mTextureCoords[0][vertexIndex].y };

				if (hasTangentsAndBitangents)
				{
					vertex.tangent = { aiMesh->mTangents[vertexIndex].x, aiMesh->mTangents[vertexIndex].y, aiMesh->mTangents[vertexIndex].z };
					vertex.bitangent = { aiMesh->mBitangents[vertexIndex].x, aiMesh->mBitangents[vertexIndex].y, aiMesh->mBitangents[vertexIndex].z };
				}
			}

			for (uint32_t faceIndex { 0U }; faceIndex < numFaces; ++faceIndex)
			{
				aiFace face{ aiMesh->mFaces[faceIndex] };

				for (uint32_t index { 0U }; index < face.mNumIndices; ++index)
					indices.push_back(face.mIndices[index]);
			}

			MeshInfo& meshInfo{ meshesInfo.emplace_back() };
			meshInfo.vertices = verticesCount;
			meshInfo.indices = numFaces * 3;
			meshInfo.materialIndex = aiMesh->mMaterialIndex;
		}

		std::vector<EntityNode> hierarchy;
		ReturnModelHierarchy(importScene, importScene->mRootNode, hierarchy);

		uint32_t totalSizeMeshes{ static_cast<uint32_t>(vertices.size() * sizeof(StaticVertex) + indices.size() * sizeof(uint32_t)) };
		uint32_t totalMeshesInfo{ static_cast<uint32_t>(meshesInfo.size() * sizeof(MeshInfo)) };
		uint32_t indicesOffset{ static_cast<uint32_t>(vertices.size() * sizeof(StaticVertex)) };

		uint32_t header[3] { totalMeshesInfo, totalSizeMeshes, indicesOffset };
		uint32_t totalBufferSize { sizeof(header) + totalMeshesInfo + totalSizeMeshes + indicesOffset };

		uint64_t totalMaterialsInfo{ static_cast<UUID>(materialsInfo.size() * sizeof(UUID)) };
		
		Buffer buffer;
		buffer.size = totalBufferSize + totalMaterialsInfo;
		buffer.data = new char[buffer.size];

		char* buf{ buffer.data };
		memcpy(buf, header, sizeof(header));
		buf += sizeof(header);

		memcpy(buf, &totalMaterialsInfo, sizeof(UUID));
		buf += sizeof(UUID);

		memcpy(buf, materialsInfo.data(), materialsInfo.size() * sizeof(UUID));
		buf += materialsInfo.size() * sizeof(UUID);

		memcpy(buf, meshesInfo.data(), meshesInfo.size() * sizeof(MeshInfo));
		buf += meshesInfo.size() * sizeof(MeshInfo);

		memcpy(buf, vertices.data(), vertices.size() * sizeof(StaticVertex));
		buf += vertices.size() * sizeof(StaticVertex);

		memcpy(buf, indices.data(), indices.size() * sizeof(uint32_t));

		FileSystem::WriteBinaryFile(aMetadata.filepath, buffer);
	}

	std::shared_ptr<Resource> ModelImporter::LoadModel(const ResourceMetadata& aMetadata)
	{
		const Buffer& buffer{ FileSystem::ReadBinaryFile(aMetadata.filepath) };

		char* data{ buffer.data };

		uint32_t totalMeshesInfo, totalSizeMeshes, indicesOffset;
		memcpy(&totalMeshesInfo, data, sizeof(uint32_t));
		data += sizeof(uint32_t);
		memcpy(&totalSizeMeshes, data, sizeof(uint32_t));
		data += sizeof(uint32_t);
		memcpy(&indicesOffset, data, sizeof(uint32_t));
		data += sizeof(uint32_t);

		UUID totalMaterialsInfo;
		memcpy(&totalMaterialsInfo, data, sizeof(UUID));
		data += sizeof(UUID);

		std::vector<UUID> materialsInfo;
		materialsInfo.resize(totalMaterialsInfo / sizeof(UUID));

		memcpy(materialsInfo.data(), data, totalMaterialsInfo);
		data += totalMaterialsInfo;

		std::vector<MeshInfo> meshesInfo;
		meshesInfo.resize(totalMeshesInfo / sizeof(MeshInfo));

		memcpy(meshesInfo.data(), data, totalMeshesInfo);
		data += totalMeshesInfo;

		std::vector<std::shared_ptr<Material>> materials;
		std::vector<std::shared_ptr<StaticMesh>> meshes;

		for (uint32_t index{ 0U }; index < materialsInfo.size(); ++index)
		{
			materials.push_back(std::static_pointer_cast<Material>(ResourceManager::GetResource(materialsInfo[index])));
		}

		char* verticesPtr{ data };
		char* indicesPtr{ data + indicesOffset };

		for (uint32_t i { 0U }; i < meshesInfo.size(); ++i)
		{
			const MeshInfo& meshInfo{ meshesInfo[i] };

			std::vector<StaticVertex> vertices;
			std::vector<uint32_t> indices;

			vertices.resize(meshInfo.vertices);
			indices.resize(meshInfo.indices);

			memcpy(vertices.data(), verticesPtr, sizeof(StaticVertex) * meshInfo.vertices);
			verticesPtr += sizeof(StaticVertex) * meshInfo.vertices;

			memcpy(indices.data(), indicesPtr, sizeof(uint32_t) * meshInfo.indices);
			indicesPtr += sizeof(uint32_t) * meshInfo.indices;

			std::shared_ptr<StaticMesh> staticMesh{ std::make_shared<StaticMesh>(vertices, indices) };
			meshes.push_back(staticMesh);
		}

		std::shared_ptr<Model> model{ std::make_shared<Model>(meshes, materials) };

		return model;
	}
	
	void ModelImporter::ReturnModelHierarchy(const aiScene* aScene, const aiNode* aNode, std::vector<EntityNode>& hierarchy)
	{
		//EntityNode& entityNode = hierarchy.emplace_back();

		//aiVector3D position, rotation, scale;
		//aNode->mTransformation.Decompose(scale, rotation, position);

		//entityNode.name = aNode->mName.C_Str();
		//entityNode.position = { position.x, position.y, position.z };
		//entityNode.rotation = { rotation.x, rotation.y, rotation.z };
		//entityNode.scale = { scale.x, scale.y, scale.z };
		//entityNode.numChildren = aNode->mNumChildren;
		//if (aNode->mNumMeshes > 0)
		//{
		//	entityNode.meshNode.meshID = aNode->mMeshes[0];
		//	entityNode.meshNode.materialID = aScene->mMeshes[aNode->mMeshes[0]]->mMaterialIndex;
		//}

		//for (uint32_t index { 0U }; index < entityNode.numChildren; ++index)
		//{
		//	ReturnModelHierarchy(aScene, aNode->mChildren[index], hierarchy);
		//}

		std::stack<aiNode*> nodes;
		nodes.push(aScene->mRootNode);

		while (!nodes.empty())
		{
			EntityNode& entityNode{ hierarchy.emplace_back() };
			const aiNode* node{ nodes.top() };

			aiVector3D position, rotation, scale;
			node->mTransformation.Decompose(scale, rotation, position);

			entityNode.position = { position.x, position.y, position.z };
			entityNode.rotation = { rotation.x, rotation.y, rotation.z };
			entityNode.scale = { scale.x, scale.y, scale.z };
			
			entityNode.name = node->mName.C_Str();
			entityNode.numChildren = node->mNumChildren;
			if (node->mNumMeshes > 0)
			{
				entityNode.meshNode.meshID = node->mMeshes[0];
				entityNode.meshNode.materialID = aScene->mMeshes[node->mMeshes[0]]->mMaterialIndex;
			}

			nodes.pop();
			for (int index { entityNode.numChildren - 1 }; index >= 0; --index)
			{
				nodes.push(node->mChildren[index]);
			}
		}
	}
}