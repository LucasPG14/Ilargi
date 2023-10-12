#include "ilargipch.h"

#include "ModelImporter.h"

#include "Resources/Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h> 

namespace Ilargi
{
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
				aiVector3D aiVertex = assimpMesh->mVertices[j];
				StaticVertex& vertex = submesh.vertices[j];

				vertex.position = { aiVertex.x, aiVertex.y, aiVertex.z };

				if (hasNormals)
				{
					aiVector3D aiNormal = assimpMesh->mNormals[j];
					vertex.normal = { aiNormal.x, aiNormal.y, aiNormal.z };
				}

				if (hasTexCoords)
				{
					aiVector3D aiTexCoord = assimpMesh->mTextureCoords[0][j];
					vertex.texCoord = { aiTexCoord.x, aiTexCoord.y };
				}

				if (hasTangentsAndBitangents)
				{
					aiVector3D aiTangent = assimpMesh->mTangents[j];
					aiVector3D aiBitangent = assimpMesh->mBitangents[j];
					vertex.tangent = { aiTangent.x, aiTangent.y, aiTangent.z };
					vertex.bitangent = { aiBitangent.x, aiBitangent.y, aiBitangent.z };
				}
			}

			for (uint32_t j = 0; j < numFaces; ++j)
			{
				aiFace face = assimpMesh->mFaces[j];

				for (uint32_t k = 0; k < face.mNumIndices; ++k)
					submesh.indices.push_back(face.mIndices[k]);
			}

			mesh->AddSubmesh(submesh);

			return mesh;
		}
	}
}