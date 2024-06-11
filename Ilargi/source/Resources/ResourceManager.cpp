#include "ilargipch.h"

#include "ResourceManager.h"
#include "Utils/Importers/ModelImporter.h"

namespace Ilargi
{
	const std::map<std::string, ResourceType> extensionsMap = 
	{
		{ ".png",		ResourceType::TEXTURE2D },
		{ ".jpg",		ResourceType::TEXTURE2D },
		{ ".jpeg",		ResourceType::TEXTURE2D },
		{ ".fbx",		ResourceType::MESH },
		{ ".obj",		ResourceType::MESH },
		//{ ".ilargi",	ResourceType::SCENE },
	};

	using ImportFn = std::function<void(const std::filesystem::path&, const std::filesystem::path&)>;
	static std::map<ResourceType, ImportFn> importers =
	{
		{ ResourceType::MESH, ModelImporter::ImportModel2 }
	};

	using LoadFn = std::function<void(const ResourceMetaData&)>;
	//static std::map<ResourceType, LoadFn> loaders =
	//{
	//	{ ResourceType::MESH, ModelImporter::LoadModel }
	//};

	std::unordered_map<UUID, ResourceMetaData> ResourceManager::resourcesMetadata;
	std::unordered_map<UUID, std::shared_ptr<Resource>> ResourceManager::loadedResources;
	
	UUID ResourceManager::ImportResource(const std::filesystem::path& path)
	{
		UUID resourceUUID;

		std::string newPath = ("assets" / path.stem()).string() + ".ires";
		
		ResourceMetaData metadata;
		metadata.type = GetResourceType(path.extension().string());
		metadata.sourceFile = path;
		metadata.filepath = newPath;

		resourcesMetadata[resourceUUID] = metadata;
		importers[metadata.type](path, newPath);

		return resourceUUID;
	}

	void ResourceManager::LoadAsset(UUID uuid)
	{
		//auto& metadata = resourcesMetadata[uuid];


	}
	
	const ResourceType ResourceManager::GetResourceType(const std::string& str)
	{
		if (extensionsMap.contains(str))
			return extensionsMap.at(str);

		return ResourceType::NONE;
	}
}