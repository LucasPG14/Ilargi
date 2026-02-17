#include "ilargipch.h"

#include "MaterialImporter.h"
#include "ModelImporter.h"
#include "Utils/FileSystem.h"

#include "Resources/Material.h"
#include "Resources/ResourceManager.h"
#include "Resources/Texture.h"
#include "Renderer/Renderer.h"

namespace Ilargi
{
	std::shared_ptr<Resource> MaterialImporter::LoadMaterial(const ResourceMetadata& aMetadata)
	{
		BinaryReader reader(aMetadata.filepath);

		MaterialHeader materialHeader;
		reader.Read(materialHeader);

		std::string shaderName;
		reader.ReadString(shaderName);

		MaterialData materialData;
		reader.Read(materialData);

		uint8_t texturesSize;
		reader.Read(texturesSize);

		const std::shared_ptr<Material>& material{ Material::Create(Renderer::GetShader(shaderName), materialData) };
		
		for (uint8_t index{ 0U }; index < texturesSize; ++index)
		{
			std::string textureName;
			reader.ReadString(textureName);
			UUID textureUUID;
			reader.Read(textureUUID);

			material->UpdateTexture(textureName, std::static_pointer_cast<Texture2D>(ResourceManager::GetResource(textureUUID)));
		}

		return material;
	}
	
	void MaterialImporter::SaveMaterial(const ResourceMetadata& aMetadata, const std::shared_ptr<Resource>& aResource)
	{
		const std::shared_ptr<Material>& material{ std::static_pointer_cast<Material>(aResource) };
		BinaryWriter writer(aMetadata.filepath.string());

		const std::shared_ptr<IShader>& shader{ material->GetShader() };
		const MaterialData& materialData{ material->GetMaterialData() };

		MaterialHeader materialHeader;

		writer.Write(materialHeader);
		writer.WriteString(shader->GetName());
		writer.Write(materialData);

		const auto& texturesMap{ material->GetTextures() };
		writer.Write(static_cast<uint8_t>(texturesMap.size()));

		for (const auto& [textureName, textureUUID] : texturesMap)
		{
			writer.WriteString(textureName);
			writer.Write(textureUUID);
		}
	}
}