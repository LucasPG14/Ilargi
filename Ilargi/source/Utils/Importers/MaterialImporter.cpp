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

		// TODO: Textures in material file
		//bool hasDiffuseTexture;
		//reader.Read(hasDiffuseTexture);
		//
		//UUID diffuse;
		//if (hasDiffuseTexture)
		//{
		//	reader.Read(diffuse);
		//}

		auto material{ Material::Create(Renderer::GetShaderLibrary()->Get(shaderName), materialData) };

		return material;
	}
	
	void MaterialImporter::SaveMaterial(const ResourceMetadata& aMetadata, const std::shared_ptr<Resource>& aResource)
	{
		const std::shared_ptr<Material>& material{ std::static_pointer_cast<Material>(aResource) };
		BinaryWriter writer(aMetadata.filepath.string());

		const std::shared_ptr<Shader>& shader{ material->GetShader() };
		const MaterialData& materialData{ material->GetMaterialData() };

		MaterialHeader materialHeader;

		writer.Write(materialHeader);
		writer.WriteString(shader->GetName());
		writer.Write(materialData);

		//bool hasDiffuseTexture{ material->GetDiffuse() != nullptr };
		//writer.Write(hasDiffuseTexture);
		//
		//if (hasDiffuseTexture)
		//{
		//	writer.Write(material->GetDiffuse()->mResourceUUID);
		//}
	}
}