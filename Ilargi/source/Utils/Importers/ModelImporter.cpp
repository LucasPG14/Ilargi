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

		const aiScene* importScene = importer.ReadFile(aMetadata.sourceFile.string().c_str(), aiProcess_CalcTangentSpace | aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

		if (!importScene)
		{
			ILG_CORE_ERROR("Couldn't import model: {0}", aMetadata.sourceFile.string());
			return;
		}

		std::vector<MaterialInfo> materialsInfo;
		materialsInfo.reserve(importScene->mNumMaterials);

		for (uint32_t materialIndex { 1U }; materialIndex < importScene->mNumMaterials; ++materialIndex)
		{
			const aiMaterial* aiMaterial = importScene->mMaterials[materialIndex];

			MaterialInfo& materialInfo = materialsInfo.emplace_back();
		}

		std::vector<MeshInfo> meshesInfo;
		meshesInfo.reserve(importScene->mNumMeshes);

		std::vector<StaticVertex> vertices;
		std::vector<uint32_t> indices;

		for (uint32_t meshIndex { 0U }; meshIndex < importScene->mNumMeshes; ++meshIndex)
		{
			const aiMesh* aiMesh = importScene->mMeshes[meshIndex];

			bool hasNormals = aiMesh->HasNormals();
			bool hasTexCoords = aiMesh->HasTextureCoords(0);
			bool hasTangentsAndBitangents = aiMesh->HasTangentsAndBitangents();

			uint32_t verticesCount = aiMesh->mNumVertices;
			uint32_t numFaces = aiMesh->mNumFaces;
			uint32_t indicesCount = numFaces * 3;

			vertices.reserve(vertices.size() + verticesCount);
			indices.reserve(indices.size() + indicesCount);

			for (uint32_t vertexIndex { 0U }; vertexIndex < verticesCount; ++vertexIndex)
			{
				StaticVertex& vertex = vertices.emplace_back();
				vertex.position = { aiMesh->mVertices[vertexIndex].x, aiMesh->mNormals[vertexIndex].y, aiMesh->mNormals[vertexIndex].z };

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
				aiFace face = aiMesh->mFaces[faceIndex];

				for (uint32_t index { 0U }; index < face.mNumIndices; ++index)
					indices.push_back(face.mIndices[index]);
			}

			MeshInfo& meshInfo = meshesInfo.emplace_back();
			meshInfo.vertices = verticesCount;
			meshInfo.indices = numFaces * 3;
			meshInfo.materialIndex = aiMesh->mMaterialIndex - 1;
		}

		uint32_t totalSizeMeshes = static_cast<uint32_t>(vertices.size() * sizeof(StaticVertex) + indices.size() * sizeof(uint32_t));
		uint32_t totalMeshesInfo = static_cast<uint32_t>(meshesInfo.size() * sizeof(MeshInfo));
		uint32_t totalMaterialsInfo = static_cast<uint32_t>(materialsInfo.size() * sizeof(MaterialInfo));
		uint32_t indicesOffset = static_cast<uint32_t>(vertices.size() * sizeof(StaticVertex));

		uint32_t header[4] { totalMaterialsInfo, totalMeshesInfo, totalSizeMeshes, indicesOffset };

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

		for (uint32_t i { 0U }; i < meshesInfo.size(); ++i)
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
}