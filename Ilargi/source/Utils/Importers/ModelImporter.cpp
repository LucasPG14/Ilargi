#include "ilargipch.h"

#include "ModelImporter.h"
#include "Utils/FileSystem.h"

#include "Scene/Scene.h"

#include "Renderer/Renderer.h"
#include "Resources/ResourceManager.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 

namespace Ilargi
{
	template<>
	constexpr vec2& vec2::operator=(const aiVector3D& v)
	{
		x = v.x;
		y = v.y;

		return *this;
	}

	template<>
	vec3& vec3::operator=(const aiVector3D& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;

		return *this;
	}

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

		std::vector<StaticVertex> vertices;
		std::vector<uint32_t> indices;

		const aiMesh* aiMesh = importScene->mMeshes[0];

		bool hasNormals = aiMesh->HasNormals();
		bool hasTexCoords = aiMesh->HasTextureCoords(0);
		bool hasTangentsAndBitangents = aiMesh->HasTangentsAndBitangents();

		uint32_t verticesCount = aiMesh->mNumVertices;
		uint32_t numFaces = aiMesh->mNumFaces;

		vertices.reserve(verticesCount);
		indices.reserve(numFaces * 3);

		for (int i = 0; i < verticesCount; ++i)
		{
			StaticVertex& vertex = vertices.emplace_back();
			vertex.position = aiMesh->mVertices[i];

			if (hasNormals)
				vertex.normal = aiMesh->mNormals[i];

			if (hasTexCoords)
				vertex.texCoord = aiMesh->mTextureCoords[0][i];

			if (hasTangentsAndBitangents)
			{
				vertex.tangent = aiMesh->mTangents[i];
				vertex.bitangent = aiMesh->mBitangents[i];
			}
		}

		for (uint32_t j = 0; j < numFaces; ++j)
		{
			aiFace face = aiMesh->mFaces[j];

			for (uint32_t k = 0; k < face.mNumIndices; ++k)
				indices.push_back(face.mIndices[k]);
		}

		int header[2] = { verticesCount, indices.size() };

		Buffer buffer;

		buffer.size = sizeof(header) + (vertices.size() * sizeof(StaticVertex)) + (indices.size() * sizeof(uint32_t));
		buffer.data = new char[buffer.size];

		char* buf = buffer.data;
		memcpy(buf, header, sizeof(header));
		buf += sizeof(header);

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