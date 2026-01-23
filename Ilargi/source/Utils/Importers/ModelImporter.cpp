#include "ilargipch.h"

#include "ModelImporter.h"
#include "MaterialImporter.h"
#include "TextureImporter.h"
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
	inline glm::mat4 ConvertToGlm(const aiMatrix4x4& aMatrix)
	{
		return {
			aMatrix.a1, aMatrix.b1, aMatrix.c1, aMatrix.d1,
			aMatrix.a2, aMatrix.b2, aMatrix.c2, aMatrix.d2,
			aMatrix.a3, aMatrix.b3, aMatrix.c3, aMatrix.d3,
			aMatrix.a4, aMatrix.b4, aMatrix.c4, aMatrix.d4
		};
	}

	void ModelImporter::ImportModel(UUID aUUID, const ResourceMetadata& aMetadata)
	{
		Assimp::Importer importer;

		const aiScene* importScene{ importer.ReadFile(aMetadata.sourceFile.string().c_str(), aiProcessPreset_TargetRealtime_MaxQuality) };

		if (!importScene)
		{
			ILG_CORE_ERROR("Couldn't import model: {0}", aMetadata.sourceFile.string());
			return;
		}

		std::vector<UUID> meshesUUIDs;
		std::vector<UUID> materialsUUIDs;

		meshesUUIDs.reserve(importScene->mNumMeshes);
		materialsUUIDs.reserve(importScene->mNumMaterials);

		for (uint32_t meshIndex{ 0U }; meshIndex < importScene->mNumMeshes; ++meshIndex)
		{
			const aiMesh* aiMesh{ importScene->mMeshes[meshIndex] };

			meshesUUIDs.emplace_back(ImportMesh(aiMesh, aMetadata));
		}

		for (uint32_t materialIndex{ 0U }; materialIndex < importScene->mNumMaterials; ++materialIndex)
		{
			aiMaterial* material{ importScene->mMaterials[materialIndex] };
			
			materialsUUIDs.emplace_back(ImportMaterial(material, aMetadata));
		}

		std::vector<ModelNode> modelNodes;
		ProcessNode(importScene, importScene->mRootNode, meshesUUIDs, materialsUUIDs, modelNodes);

		ModelHeader modelHeader{};
		modelHeader.childrens = static_cast<uint32_t>(modelNodes.size());

		BinaryWriter writer(aMetadata.filepath);
		writer.Write(modelHeader);

		for (const auto& modelNode : modelNodes)
		{
			writer.WriteVector(modelNode.submeshes);
			writer.WriteString(modelNode.name);
			writer.WriteVector(modelNode.childrens);
			writer.Write(modelNode.localTransform);
		}
	}

	std::shared_ptr<Resource> ModelImporter::LoadModel(const ResourceMetadata& aMetadata)
	{
		BinaryReader reader(aMetadata.filepath);

		ModelHeader modelHeader;
		reader.Read(modelHeader);
		
		std::vector<ModelNode> modelNodes(modelHeader.childrens);
		
		for (auto& node : modelNodes)
		{
			reader.ReadVector(node.submeshes);
			reader.ReadString(node.name);
			reader.ReadVector(node.childrens);
			reader.Read(node.localTransform);
		}

		std::shared_ptr<Model> model{ std::make_shared<Model>(modelNodes) };

		return model;
	}

	std::shared_ptr<Resource> ModelImporter::LoadMesh(const ResourceMetadata& aMetadata)
	{
		BinaryReader reader(aMetadata.filepath);

		std::vector<StaticVertex> vertices;
		std::vector<uint32_t> indices;

		MeshHeader meshHeader;
		reader.Read(meshHeader);

		vertices.resize(meshHeader.verticesCount);
		indices.resize(meshHeader.indicesCount);

		reader.Read(vertices.data(), vertices.size() * sizeof(StaticVertex));
		reader.Read(indices.data(), indices.size() * sizeof(uint32_t));

		return std::make_shared<StaticMesh>(vertices, indices);
	}

	UUID ModelImporter::ImportMesh(const aiMesh* aMesh, const ResourceMetadata& aMetadata)
	{
		std::vector<StaticVertex> vertices;
		std::vector<uint32_t> indices;

		MeshHeader meshHeader;
		meshHeader.verticesCount = aMesh->mNumVertices;
		meshHeader.indicesCount = aMesh->mNumFaces * 3;
		vertices.reserve(meshHeader.verticesCount);
		indices.reserve(meshHeader.indicesCount);

		glm::vec3 bitangent;
		glm::vec3 tangent;

		for (uint32_t vertexIndex{ 0U }; vertexIndex < meshHeader.verticesCount; ++vertexIndex)
		{
			StaticVertex& vertex{ vertices.emplace_back() };
			vertex.position = glm::vec3(aMesh->mVertices[vertexIndex].x, aMesh->mVertices[vertexIndex].y, aMesh->mVertices[vertexIndex].z);
			vertex.normal = glm::vec3(aMesh->mNormals[vertexIndex].x, aMesh->mNormals[vertexIndex].y, aMesh->mNormals[vertexIndex].z);
			
			bitangent = glm::vec3(aMesh->mBitangents[vertexIndex].x, aMesh->mBitangents[vertexIndex].y, aMesh->mBitangents[vertexIndex].z);
			tangent = glm::vec3(aMesh->mTangents[vertexIndex].x, aMesh->mTangents[vertexIndex].y, aMesh->mTangents[vertexIndex].z);
			
			vertex.tangent = glm::vec4(tangent, (glm::dot(glm::cross(vertex.normal, tangent), bitangent) < 0.0f) ? -1.0f : 1.0f);
			vertex.texCoord = glm::vec2(aMesh->mTextureCoords[0][vertexIndex].x, aMesh->mTextureCoords[0][vertexIndex].y);
		}

		for (uint32_t facesIndex{ 0U }; facesIndex < aMesh->mNumFaces; ++facesIndex)
		{
			const aiFace aiFace{ aMesh->mFaces[facesIndex] };
			for (uint32_t index{ 0U }; index < aiFace.mNumIndices; ++index)
			{
				indices.push_back(aiFace.mIndices[index]);
			}
		}

		std::filesystem::path meshFilepath{ aMetadata.filepath.parent_path() / std::string(aMesh->mName.C_Str() + std::string(".imesh")) };

		BinaryWriter writer(meshFilepath);
		writer.Write(meshHeader);
		writer.Write(vertices);
		writer.Write(indices);

		ResourceMetadata meshMetadata;
		meshMetadata.type = ResourceType::MESH;
		meshMetadata.sourceFile = aMetadata.sourceFile;
		meshMetadata.filepath = meshFilepath;
		meshMetadata.lastWriteTime = std::filesystem::last_write_time(meshMetadata.filepath);
		
		return ResourceManager::RegisterResource(meshMetadata);
	}

	UUID ModelImporter::ImportMaterial(const aiMaterial* aMaterial, const ResourceMetadata& aMetadata)
	{
		MaterialHeader materialHeader{};

		glm::vec4 materialColor{};
		aiColor4D color;
		if (aiGetMaterialColor(aMaterial, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS)
		{
			materialColor = { color.r, color.g, color.b, color.a };
		}
		aiString materialName;
		if (aMaterial->Get(AI_MATKEY_NAME, materialName) != AI_SUCCESS)
		{
			ILG_CORE_ERROR("Unable to find the name of the material");
		}
		
		// TODO: Fix and automatize this
		std::unordered_map<std::string, UUID> texturesUUID;
		aiString materialBaseColorTexture;
		if (aiGetMaterialTexture(aMaterial, aiTextureType_BASE_COLOR, 0, &materialBaseColorTexture) == AI_SUCCESS)
		{
			ResourceMetadata textureMetadata;
			textureMetadata.type = ResourceType::TEXTURE2D;
			textureMetadata.sourceFile = aMetadata.sourceFile.parent_path() / materialBaseColorTexture.C_Str();
			textureMetadata.filepath = aMetadata.filepath.parent_path() / (std::filesystem::path(materialBaseColorTexture.C_Str()).stem().string() + ".itex");
			texturesUUID["DiffuseMap"] = ResourceManager::RegisterResource(textureMetadata);
			TextureImporter::ImportTexture(texturesUUID["DiffuseMap"], textureMetadata);
		}

		aiString materialNormalTexture;
		if (aiGetMaterialTexture(aMaterial, aiTextureType_NORMALS, 0, &materialNormalTexture) == AI_SUCCESS)
		{
			ResourceMetadata textureMetadata;
			textureMetadata.type = ResourceType::TEXTURE2D;
			textureMetadata.sourceFile = aMetadata.sourceFile.parent_path() / materialNormalTexture.C_Str();
			textureMetadata.filepath = aMetadata.filepath.parent_path() / (std::filesystem::path(materialNormalTexture.C_Str()).stem().string() + ".itex");
			texturesUUID["NormalMap"] = ResourceManager::RegisterResource(textureMetadata);
			TextureImporter::ImportTexture(texturesUUID["NormalMap"], textureMetadata);
		}

		aiString materialMetallicTexture;
		if (aiGetMaterialTexture(aMaterial, aiTextureType_METALNESS, 0, &materialMetallicTexture) == AI_SUCCESS)
		{
			ResourceMetadata textureMetadata;
			textureMetadata.type = ResourceType::TEXTURE2D;
			textureMetadata.sourceFile = aMetadata.sourceFile.parent_path() / materialMetallicTexture.C_Str();
			textureMetadata.filepath = aMetadata.filepath.parent_path() / (std::filesystem::path(materialMetallicTexture.C_Str()).stem().string() + ".itex");
			texturesUUID["MetallicMap"] = ResourceManager::RegisterResource(textureMetadata);
			TextureImporter::ImportTexture(texturesUUID["MetallicMap"], textureMetadata);
		}

		aiString materialRoughnessTexture;
		if (aiGetMaterialTexture(aMaterial, aiTextureType_METALNESS, 0, &materialRoughnessTexture) == AI_SUCCESS)
		{
			ResourceMetadata textureMetadata;
			textureMetadata.type = ResourceType::TEXTURE2D;
			textureMetadata.sourceFile = aMetadata.sourceFile.parent_path() / materialRoughnessTexture.C_Str();
			textureMetadata.filepath = aMetadata.filepath.parent_path() / (std::filesystem::path(materialRoughnessTexture.C_Str()).stem().string() + ".itex");
			texturesUUID["RoughnessMap"] = ResourceManager::RegisterResource(textureMetadata);
			TextureImporter::ImportTexture(texturesUUID["RoughnessMap"], textureMetadata);
		}

		std::filesystem::path materialFilepath { aMetadata.filepath.parent_path() / std::string(materialName.C_Str() + std::string(".imat")) };

		MaterialData materialData;
		materialData.color = { color.r, color.g, color.g, color.a };

		BinaryWriter writer(materialFilepath.string());
		writer.Write(materialHeader);
		writer.WriteString(std::string("PBR_Static"));
		writer.Write(materialData);

		uint8_t texturesSize{ static_cast<uint8_t>(texturesUUID.size()) };
		writer.Write(texturesSize);
		
		for (const auto& [textureName, textureUUID] : texturesUUID)
		{
			writer.WriteString(textureName);
			writer.Write(textureUUID);
		}

		ResourceMetadata materialMetadata;
		materialMetadata.type = ResourceType::MATERIAL;
		materialMetadata.sourceFile = aMetadata.sourceFile;
		materialMetadata.filepath = materialFilepath;
		materialMetadata.lastWriteTime = std::filesystem::last_write_time(materialMetadata.filepath);

		return ResourceManager::RegisterResource(materialMetadata);
	}

	uint32_t ModelImporter::ProcessNode(const aiScene* aScene, const aiNode* aNode, const std::vector<UUID>& aMeshesUUIDs, const std::vector<UUID>& aMaterialsUUIDs, std::vector<ModelNode>& aModelNodes)
	{
		size_t nodeIndex{ aModelNodes.size() };

		ModelNode& modelNode { aModelNodes.emplace_back() };
		modelNode.name = aNode->mName.C_Str();
		for (uint32_t index{ 0U }; index < aNode->mNumMeshes; ++index)
		{
			StaticSubmesh submesh;
			submesh.mesh = aMeshesUUIDs[aNode->mMeshes[index]];
			submesh.material = aMaterialsUUIDs[aScene->mMeshes[aNode->mMeshes[index]]->mMaterialIndex];
			modelNode.submeshes.push_back(submesh);
		}
		
		modelNode.localTransform = ConvertToGlm(aNode->mTransformation);

		for (uint32_t index{ 0U }; index < aNode->mNumChildren; ++index)
		{
			uint32_t childrenIndex{ ProcessNode(aScene, aNode->mChildren[index], aMeshesUUIDs, aMaterialsUUIDs, aModelNodes) };
			aModelNodes[nodeIndex].childrens.push_back(childrenIndex);
		}

		return static_cast<uint32_t>(nodeIndex);
	}
}