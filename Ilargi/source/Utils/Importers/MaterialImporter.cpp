#include "ilargipch.h"

#include "MaterialImporter.h"
#include "Utils/FileSystem.h"

#include "Resources/Material.h"
#include "Resources/ResourceManager.h"
#include "Resources/Texture.h"
#include "Renderer/Renderer.h"

namespace Ilargi
{
	std::shared_ptr<Resource> MaterialImporter::LoadMaterial(const ResourceMetadata& aMetadata)
	{
		const Buffer& buffer { FileSystem::ReadBinaryFile(aMetadata.filepath) };

		uint32_t shaderNameSize { 0U };
		std::string shaderName;

		char* buf{ buffer.data };
		memcpy(&shaderNameSize, buf, sizeof(uint32_t));
		buf += sizeof(uint32_t);

		shaderName.resize(shaderNameSize);
		memcpy(shaderName.data(), buf, shaderNameSize * sizeof(char));
		buf += shaderNameSize * sizeof(char);

		MaterialData materialData {};
		memcpy(&materialData, buf, sizeof(MaterialData));
		buf += sizeof(MaterialData);

		bool hasDiffuseTexture{true};
		memcpy(&hasDiffuseTexture, buf, sizeof(bool));
		buf += sizeof(bool);

		UUID diffuse{};
		if (hasDiffuseTexture)
		{
			memcpy(&diffuse, buf, sizeof(UUID));
			buf += sizeof(UUID);
		}

		auto material{ Material::Create(Renderer::GetShaderLibrary()->Get(shaderName), materialData) };
		material->UpdateDiffuse(std::static_pointer_cast<Texture2D>(ResourceManager::GetResource(diffuse)));

		return material;
	}
	
	void MaterialImporter::SaveMaterial(const ResourceMetadata& aMetadata, const std::shared_ptr<Resource>& aResource)
	{
		const std::shared_ptr<Material>& material{ std::static_pointer_cast<Material>(aResource) };
		Buffer buffer;

		const std::shared_ptr<Shader>& shader{ material->GetShader() };
		const MaterialData& materialData{ material->GetMaterialData() };

		uint32_t shaderNameSize{ (uint32_t)shader->GetName().length() };

		buffer.size = sizeof(uint32_t) + shaderNameSize + sizeof(materialData) + sizeof(UUID);
		buffer.data = new char[buffer.size];

		char* buf{ buffer.data };
		memcpy(buf, &shaderNameSize, sizeof(uint32_t));
		buf += sizeof(uint32_t);

		memcpy(buf, shader->GetName().data(), shaderNameSize * sizeof(char));
		buf += shaderNameSize * sizeof(char);

		memcpy(buf, &materialData, sizeof(materialData));
		buf += sizeof(materialData);

		bool hasDiffuseTexture{ material->GetDiffuse() != nullptr };
		memcpy(buf, &hasDiffuseTexture, sizeof(bool));
		buf += sizeof(bool);

		if (hasDiffuseTexture)
		{
			memcpy(buf, &material->GetDiffuse()->mResourceUUID, sizeof(UUID));
			buf += sizeof(UUID);
		}

		FileSystem::WriteBinaryFile(aMetadata.filepath, buffer);
	}
}