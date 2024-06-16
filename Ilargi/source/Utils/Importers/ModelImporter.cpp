#include "ilargipch.h"

#include "ModelImporter.h"
#include "Utils/FileSystem.h"

#include "Scene/Scene.h"

#include "Renderer/Renderer.h"
#include "Resources/ResourceManager.h"
#include "Resources/Mesh.h"

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

	std::shared_ptr<StaticMesh> ModelImporter::ImportModel(const std::string path)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate | 
			aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

		if (!scene)
		{
			ILG_CORE_ERROR("Couldn't import model: {0}", path);
			return nullptr;
		}

		uint32_t meshCount = scene->mNumMeshes;

		std::shared_ptr<StaticMesh> mesh = std::make_shared<StaticMesh>(meshCount);
		for (uint32_t i = 0; i < meshCount; ++i)
		{
			// Saving all the necessary variables
			aiMesh* assimpMesh = scene->mMeshes[i];
			uint32_t verticesCount = assimpMesh->mNumVertices;
			uint32_t numFaces = assimpMesh->mNumFaces;

			bool hasNormals = assimpMesh->HasNormals();
			bool hasTexCoords = assimpMesh->HasTextureCoords(0);
			bool hasTangentsAndBitangents = assimpMesh->HasTangentsAndBitangents();

			StaticSubmesh submesh;
			submesh.vertices.resize(verticesCount);
			submesh.indices.reserve(numFaces * 3); // * 3 because it's a triangle

			for (uint32_t j = 0; j < verticesCount; ++j)
			{
				StaticVertex& vertex = submesh.vertices[j];

				//aiVector3D aiVertex = assimpMesh->mVertices[j];
				vertex.position = assimpMesh->mVertices[j];

				if (hasNormals)
				{
					//aiVector3D aiNormal ;
					//vertex.normal = { aiNormal.x, aiNormal.y, aiNormal.z };
					vertex.normal = assimpMesh->mNormals[j];
				}

				if (hasTexCoords)
				{
					//aiVector3D aiTexCoord = assimpMesh->mTextureCoords[0][j];
					//vertex.texCoord = { aiTexCoord.x, aiTexCoord.y };
					vertex.texCoord = assimpMesh->mTextureCoords[0][j];
				}

				if (hasTangentsAndBitangents)
				{
					//aiVector3D aiTangent = assimpMesh->mTangents[j];
					//aiVector3D aiBitangent = assimpMesh->mBitangents[j];
					//vertex.tangent = { aiTangent.x, aiTangent.y, aiTangent.z };
					//vertex.bitangent = { aiBitangent.x, aiBitangent.y, aiBitangent.z };
					vertex.tangent = assimpMesh->mTangents[j];
					vertex.bitangent = assimpMesh->mBitangents[j];
				}
			}

			for (uint32_t j = 0; j < numFaces; ++j)
			{
				aiFace face = assimpMesh->mFaces[j];

				for (uint32_t k = 0; k < face.mNumIndices; ++k)
					submesh.indices.push_back(face.mIndices[k]);
			}

			mesh->AddSubmesh(submesh);
		}

		return mesh;
	}
	
	void ModelImporter::ImportModel2(const std::filesystem::path& path, const std::filesystem::path& assetsPath)
	{
		Assimp::Importer importer;
		
		const aiScene* scene = importer.ReadFile(path.string().c_str(), aiProcess_CalcTangentSpace | aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
		
		if (!scene)
		{
			ILG_CORE_ERROR("Couldn't import model: {0}", path.string());
			return;
		}
		
		uint32_t meshCount = scene->mNumMeshes;
		std::vector<uint64_t> meshesArray(meshCount);

		//std::shared_ptr<StaticMesh> mesh = std::make_shared<StaticMesh>(meshCount);
		for (uint32_t i = 0; i < meshCount; ++i)
		{
			// Saving all the necessary variables
			aiMesh* assimpMesh = scene->mMeshes[i];
			uint32_t verticesCount = assimpMesh->mNumVertices;
			uint32_t numFaces = assimpMesh->mNumFaces;
			
			bool hasNormals = assimpMesh->HasNormals();
			bool hasTexCoords = assimpMesh->HasTextureCoords(0);
			bool hasTangentsAndBitangents = assimpMesh->HasTangentsAndBitangents();

			StaticSubmesh submesh;
			submesh.vertices.resize(verticesCount);
			submesh.indices.reserve(numFaces * 3); // * 3 because it's a triangle

			for (uint32_t j = 0; j < verticesCount; ++j)
			{
				StaticVertex& vertex = submesh.vertices[j];

				vertex.position = assimpMesh->mVertices[j];

				if (hasNormals)
					vertex.normal = assimpMesh->mNormals[j];

				if (hasTexCoords)
					vertex.texCoord = assimpMesh->mTextureCoords[0][j];

				if (hasTangentsAndBitangents)
				{
					vertex.tangent = assimpMesh->mTangents[j];
					vertex.bitangent = assimpMesh->mBitangents[j];
				}
			}

			for (uint32_t j = 0; j < numFaces; ++j)
			{
				aiFace face = assimpMesh->mFaces[j];

				for (uint32_t k = 0; k < face.mNumIndices; ++k)
					submesh.indices.push_back(face.mIndices[k]);
			}

			Buffer buffer;
			buffer.size = 8 + submesh.vertices.size() * sizeof(StaticVertex) + submesh.indices.size() * sizeof(uint32_t);
			buffer.data = new char[8 + buffer.size];

			char* pointer = (char*)buffer.data;
			uint32_t header[2] = { submesh.vertices.size(), submesh.indices.size() };

			memcpy(pointer, header, 2 * sizeof(uint32_t));
			pointer += 2 * sizeof(uint32_t);

			uint32_t verticesSize = header[0] * sizeof(StaticVertex);
			memcpy(pointer, submesh.vertices.data(), verticesSize);
			pointer += verticesSize;

			uint32_t indicesSize = header[1] * sizeof(uint32_t);
			memcpy(buffer.data, submesh.indices.data(), submesh.indices.size());

			FileSystem::WriteBinaryFile(assetsPath, buffer);
		}

		Buffer modelData;

		//aiNode* rootNode = scene->mRootNode->mChildren[0];
		//EntityNode node;
		
		//TransformNode transNode;
		//transNode.position = { 0.0f, 0.0f, 0.0f };
		//transNode.rotation = { 0.0f, 0.0f, 0.0f };
		//transNode.scale = { 1.0f, 1.0f, 1.0f };
		//node.components.push_back(transNode);

		//MeshNode meshNode;
		//meshNode.uuid = UUID();
		//node.components.push_back(meshNode);

		//modelData.size = sizeof(node);
		//modelData.data = new char[modelData.size];

		//memcpy(modelData.data, &node, modelData.size);

		//FileSystem::WriteBinaryFile(assetsPath, modelData);
	}

	void ModelImporter::ImportFBX(const std::filesystem::path& path, const std::shared_ptr<Scene>& scene)
	{
		Assimp::Importer importer;

		const aiScene* importScene = importer.ReadFile(path.string().c_str(), aiProcess_CalcTangentSpace | aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

		if (!importScene)
		{
			ILG_CORE_ERROR("Couldn't import model: {0}", path.string());
			return;
		}

		uint32_t meshCount = importScene->mNumMeshes;
		std::vector<uint64_t> meshesArray(meshCount);

		std::shared_ptr<StaticMesh> mesh = std::make_shared<StaticMesh>(meshCount);
		for (uint32_t i = 0; i < meshCount; ++i)
		{
			const Entity entity = scene->CreateEntity();

			// Saving all the necessary variables
			aiMesh* assimpMesh = importScene->mMeshes[i];
			uint32_t verticesCount = assimpMesh->mNumVertices;
			uint32_t numFaces = assimpMesh->mNumFaces;

			bool hasNormals = assimpMesh->HasNormals();
			bool hasTexCoords = assimpMesh->HasTextureCoords(0);
			bool hasTangentsAndBitangents = assimpMesh->HasTangentsAndBitangents();

			StaticSubmesh submesh;
			submesh.vertices.resize(verticesCount);
			submesh.indices.reserve(numFaces * 3); // * 3 because it's a triangle

			for (uint32_t j = 0; j < verticesCount; ++j)
			{
				StaticVertex& vertex = submesh.vertices[j];

				vertex.position = assimpMesh->mVertices[j];

				if (hasNormals)
					vertex.normal = assimpMesh->mNormals[j];

				if (hasTexCoords)
					vertex.texCoord = assimpMesh->mTextureCoords[0][j];

				if (hasTangentsAndBitangents)
				{
					vertex.tangent = assimpMesh->mTangents[j];
					vertex.bitangent = assimpMesh->mBitangents[j];
				}
			}

			for (uint32_t j = 0; j < numFaces; ++j)
			{
				aiFace face = assimpMesh->mFaces[j];

				for (uint32_t k = 0; k < face.mNumIndices; ++k)
					submesh.indices.push_back(face.mIndices[k]);
			}

			mesh->AddSubmesh(submesh);
			mesh->CreateMaterial(Renderer::GetShaderLibrary()->Get("PBR_Static"));

			scene->CreateComponent<StaticMeshComponent>(entity, mesh);
		}
	}
}