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
		//Assimp::Importer importer;
		//
		//const aiScene* importScene = importer.ReadFile(aMetadata.sourceFile.string().c_str(), aiProcess_CalcTangentSpace | aiProcess_Triangulate |
		//	aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
		//
		//if (!importScene)
		//{
		//	ILG_CORE_ERROR("Couldn't import model: {0}", aMetadata.sourceFile.string());
		//	return;
		//}
		//
		//std::vector<StaticVertex> vertices;
		//std::vector<uint32_t> indices;
		//
		//const aiMesh* aiMesh = importScene->mMeshes[0];
		//
		//bool hasNormals = aiMesh->HasNormals();
		//bool hasTexCoords = aiMesh->HasTextureCoords(0);
		//bool hasTangentsAndBitangents = aiMesh->HasTangentsAndBitangents();
		//
		//uint32_t verticesCount = aiMesh->mNumVertices;
		//uint32_t numFaces = aiMesh->mNumFaces;
		//
		//vertices.reserve(verticesCount);
		//indices.reserve(numFaces * 3);
		//
		//for (int i = 0; i < verticesCount; ++i)
		//{
		//	StaticVertex& vertex = vertices.emplace_back();
		//	vertex.position = aiMesh->mVertices[i];
		//
		//	if (hasNormals)
		//		vertex.normal = aiMesh->mNormals[i];
		//
		//	if (hasTexCoords)
		//		vertex.texCoord = aiMesh->mTextureCoords[0][i];
		//
		//	if (hasTangentsAndBitangents)
		//	{
		//		vertex.tangent = aiMesh->mTangents[i];
		//		vertex.bitangent = aiMesh->mBitangents[i];
		//	}
		//}
		//
		//for (uint32_t j = 0; j < numFaces; ++j)
		//{
		//	aiFace face = aiMesh->mFaces[j];
		//
		//	for (uint32_t k = 0; k < face.mNumIndices; ++k)
		//		indices.push_back(face.mIndices[k]);
		//}
		//
		//int header[2] = { verticesCount, indices.size() };
		//
		//Buffer buffer;
		//
		//buffer.size = sizeof(header) + (vertices.size() * sizeof(StaticVertex)) + (indices.size() * sizeof(uint32_t));
		//buffer.data = new char[buffer.size];
		//
		//char* buf = buffer.data;
		//memcpy(buf, header, sizeof(header));
		//buf += sizeof(header);
		//
		//memcpy(buf, vertices.data(), vertices.size() * sizeof(StaticVertex));
		//buf += vertices.size() * sizeof(StaticVertex);
		//
		//memcpy(buf, indices.data(), indices.size() * sizeof(uint32_t));
		//
		//FileSystem::WriteBinaryFile(aMetadata.filepath, buffer);
	}

	void ModelImporter::ImportModel2(UUID aUUID, const ResourceMetadata& aMetadata)
	{
		Assimp::Importer importer;

		const aiScene* importScene = importer.ReadFile(aMetadata.sourceFile.string().c_str(), aiProcess_CalcTangentSpace | aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

		if (!importScene)
		{
			ILG_CORE_ERROR("Couldn't import model: {0}", aMetadata.sourceFile.string());
			return;
		}

		std::vector<MaterialInfo> materialsInfo;
		materialsInfo.reserve(importScene->mNumMaterials);

		for (int i = 1; i < importScene->mNumMaterials; ++i)
		{
			const aiMaterial* aiMaterial = importScene->mMaterials[i];

			MaterialInfo& materialInfo = materialsInfo.emplace_back();
		}

		std::vector<MeshInfo> meshesInfo;
		meshesInfo.reserve(importScene->mNumMeshes);

		std::vector<StaticVertex> vertices;
		std::vector<uint32_t> indices;

		for (int i = 0; i < importScene->mNumMeshes; ++i)
		{
			const aiMesh* aiMesh = importScene->mMeshes[i];

			bool hasNormals = aiMesh->HasNormals();
			bool hasTexCoords = aiMesh->HasTextureCoords(0);
			bool hasTangentsAndBitangents = aiMesh->HasTangentsAndBitangents();

			uint32_t verticesCount = aiMesh->mNumVertices;
			uint32_t numFaces = aiMesh->mNumFaces;
			uint32_t indicesCount = numFaces * 3;

			vertices.reserve(vertices.size() + verticesCount);
			indices.reserve(indices.size() + indicesCount);

			for (int i = 0; i < verticesCount; ++i)
			{
				StaticVertex& vertex = vertices.emplace_back();
				vertex.position = { aiMesh->mVertices[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z };

				if (hasNormals)
					vertex.normal = { aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z };

				if (hasTexCoords)
					vertex.texCoord = { aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y };

				if (hasTangentsAndBitangents)
				{
					vertex.tangent = { aiMesh->mTangents[i].x, aiMesh->mTangents[i].y, aiMesh->mTangents[i].z };
					vertex.bitangent = { aiMesh->mBitangents[i].x, aiMesh->mBitangents[i].y, aiMesh->mBitangents[i].z };
				}
			}

			for (uint32_t j = 0; j < numFaces; ++j)
			{
				aiFace face = aiMesh->mFaces[j];

				for (uint32_t k = 0; k < face.mNumIndices; ++k)
					indices.push_back(face.mIndices[k]);
			}

			MeshInfo& meshInfo = meshesInfo.emplace_back();
			meshInfo.vertices = verticesCount;
			meshInfo.indices = numFaces * 3;
			meshInfo.materialIndex = aiMesh->mMaterialIndex - 1;
		}

		uint32_t totalSizeMeshes = vertices.size() * sizeof(StaticVertex) + indices.size() * sizeof(uint32_t);
		uint32_t totalMeshesInfo = meshesInfo.size() * sizeof(MeshInfo);
		uint32_t totalMaterialsInfo = materialsInfo.size() * sizeof(MaterialInfo);
		uint32_t indicesOffset = vertices.size() * sizeof(StaticVertex);

		int header[4] { totalMaterialsInfo, totalMeshesInfo, totalSizeMeshes, indicesOffset };

		uint32_t totalBufferSize { sizeof(header) + totalMaterialsInfo + totalMeshesInfo + totalSizeMeshes + indicesOffset };

		Buffer buffer;
		buffer.size = totalBufferSize;
		buffer.data = new char[buffer.size];

		char* buf = buffer.data;
		memcpy(buf, header, sizeof(header));
		buf += sizeof(header);

		memcpy(buf, materialsInfo.data(), materialsInfo.size() * sizeof(MaterialInfo));
		buf += materialsInfo.size() * sizeof(MaterialInfo);

		memcpy(buf, meshesInfo.data(), meshesInfo.size() * sizeof(MeshInfo));
		buf += meshesInfo.size() * sizeof(MeshInfo);

		memcpy(buf, vertices.data(), vertices.size() * sizeof(StaticVertex));
		buf += vertices.size() * sizeof(StaticVertex);

		memcpy(buf, indices.data(), indices.size() * sizeof(uint32_t));

		FileSystem::WriteBinaryFile(aMetadata.filepath, buffer);
	}

	std::shared_ptr<Resource> ModelImporter::LoadModel(const ResourceMetadata& aMetadata)
	{
		const Buffer& buffer = FileSystem::ReadBinaryFile(aMetadata.filepath);

		char* data = buffer.data;

		int verticesCount, indicesCount;
		memcpy(&verticesCount, data, sizeof(int));
		data += sizeof(int);
		memcpy(&indicesCount, data, sizeof(int));
		data += sizeof(int);

		std::vector<StaticVertex> vertices;
		std::vector<uint32_t> indices;
		vertices.resize(verticesCount);
		indices.resize(indicesCount);

		memcpy(vertices.data(), data, verticesCount * sizeof(StaticVertex));
		data += verticesCount * sizeof(StaticVertex);
		memcpy(indices.data(), data, indicesCount * sizeof(uint32_t));

		std::shared_ptr<StaticMesh> mesh = std::make_shared<StaticMesh>(vertices, indices);

		return mesh;
	}

	std::shared_ptr<Resource> ModelImporter::LoadModel2(const ResourceMetadata& aMetadata)
	{
		const Buffer& buffer = FileSystem::ReadBinaryFile(aMetadata.filepath);

		char* data = buffer.data;

		int totalMaterialsInfo, totalMeshesInfo, totalSizeMeshes, indicesOffset;
		memcpy(&totalMaterialsInfo, data, sizeof(int));
		data += sizeof(int);
		memcpy(&totalMeshesInfo, data, sizeof(int));
		data += sizeof(int);
		memcpy(&totalSizeMeshes, data, sizeof(int));
		data += sizeof(int);
		memcpy(&indicesOffset, data, sizeof(int));
		data += sizeof(int);

		std::vector<MaterialInfo> materialsInfo;
		materialsInfo.resize(totalMaterialsInfo / sizeof(MaterialInfo));

		memcpy(materialsInfo.data(), data, totalMaterialsInfo);
		data += totalMaterialsInfo;

		std::vector<MeshInfo> meshesInfo;
		meshesInfo.resize(totalMeshesInfo / sizeof(MeshInfo));

		memcpy(meshesInfo.data(), data, totalMeshesInfo);
		data += totalMeshesInfo;

		std::vector<std::shared_ptr<StaticMesh>> meshes;

		char* verticesPtr = data;
		char* indicesPtr = data + indicesOffset;

		for (int i = 0; i < meshesInfo.size(); ++i)
		{
			const MeshInfo& meshInfo = meshesInfo[i];

			std::vector<StaticVertex> vertices;
			std::vector<uint32_t> indices;

			vertices.resize(meshInfo.vertices);
			indices.resize(meshInfo.indices);

			memcpy(vertices.data(), verticesPtr, sizeof(StaticVertex) * meshInfo.vertices);
			verticesPtr += sizeof(StaticVertex) * meshInfo.vertices;

			memcpy(indices.data(), indicesPtr, sizeof(uint32_t) * meshInfo.indices);
			indicesPtr += sizeof(uint32_t) * meshInfo.indices;

			std::shared_ptr<StaticMesh> staticMesh = std::make_shared<StaticMesh>(vertices, indices);
			meshes.push_back(staticMesh);
		}

		std::shared_ptr<Model> model = std::make_shared<Model>(meshes);

		return model;
	}

	void ModelImporter::ImportFBX(const std::filesystem::path& aFilepath, const std::shared_ptr<Scene>& aScene)
	{
		//Assimp::Importer importer;
		//
		//const aiScene* importScene = importer.ReadFile(path.string().c_str(), aiProcess_CalcTangentSpace | aiProcess_Triangulate |
		//	aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
		//
		//if (!importScene)
		//{
		//	ILG_CORE_ERROR("Couldn't import model: {0}", path.string());
		//	return;
		//}
		//
		//uint32_t meshCount = importScene->mNumMeshes;
		//std::vector<uint64_t> meshesArray(meshCount);
		//
		//std::shared_ptr<StaticMesh> mesh = std::make_shared<StaticMesh>(meshCount);
		//for (uint32_t i = 0; i < meshCount; ++i)
		//{
		//	const Entity entity = scene->CreateEntity();
		//
		//	// Saving all the necessary variables
		//	aiMesh* assimpMesh = importScene->mMeshes[i];
		//	uint32_t verticesCount = assimpMesh->mNumVertices;
		//	uint32_t numFaces = assimpMesh->mNumFaces;
		//
		//	bool hasNormals = assimpMesh->HasNormals();
		//	bool hasTexCoords = assimpMesh->HasTextureCoords(0);
		//	bool hasTangentsAndBitangents = assimpMesh->HasTangentsAndBitangents();
		//
		//	StaticSubmesh submesh;
		//	submesh.vertices.resize(verticesCount);
		//	submesh.indices.reserve(numFaces * 3); // * 3 because it's a triangle
		//
		//	for (uint32_t j = 0; j < verticesCount; ++j)
		//	{
		//		StaticVertex& vertex = submesh.vertices[j];
		//
		//		vertex.position = assimpMesh->mVertices[j];
		//
		//		if (hasNormals)
		//			vertex.normal = assimpMesh->mNormals[j];
		//
		//		if (hasTexCoords)
		//			vertex.texCoord = assimpMesh->mTextureCoords[0][j];
		//
		//		if (hasTangentsAndBitangents)
		//		{
		//			vertex.tangent = assimpMesh->mTangents[j];
		//			vertex.bitangent = assimpMesh->mBitangents[j];
		//		}
		//	}
		//
		//	for (uint32_t j = 0; j < numFaces; ++j)
		//	{
		//		aiFace face = assimpMesh->mFaces[j];
		//
		//		for (uint32_t k = 0; k < face.mNumIndices; ++k)
		//			submesh.indices.push_back(face.mIndices[k]);
		//	}
		//
		//	mesh->AddSubmesh(submesh);
		//
		//	ResourceMetadata materialMetadata;
		//	materialMetadata.type = ResourceType::MATERIAL;
		//	materialMetadata.sourceFile = path;
		//	materialMetadata.filepath = "NewMaterial.ires";
		//
		//	std::shared_ptr<Material> material = Material::Create(Renderer::GetShaderLibrary()->Get("PBR_Static"));
		//
		//	ResourceManager::RegisterResource(materialMetadata);
		//
		//	mesh->CreateMaterial(material);
		//
		//	scene->CreateComponent<StaticMeshComponent>(entity, mesh);
		//}
	}
}