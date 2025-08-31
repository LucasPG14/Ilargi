#include "ilargipch.h"

#include "MaterialImporter.h"
#include "Utils/FileSystem.h"

#include "Resources/Material.h"
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
		buf += sizeof(uint32_t);

		MaterialData materialData;
		materialData.color = { 1.0f, 0.0f, 0.0f, 1.0f };
		materialData.metallic = 1.0f;
		materialData.roughness = 0.0f;

		return Material::Create(Renderer::GetShaderLibrary()->Get(shaderName), materialData);
	}
}