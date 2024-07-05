#include "ilargipch.h"

#include "ResourceManager.h"
#include "Utils/Importers/ModelImporter.h"

#include <ArduinoJson-v7.0.4.h>

namespace Ilargi
{
	const std::map<std::string, ResourceType> extensionsMap = 
	{
		{ ".png",		ResourceType::TEXTURE2D },
		{ ".jpg",		ResourceType::TEXTURE2D },
		{ ".jpeg",		ResourceType::TEXTURE2D },
		{ ".fbx",		ResourceType::MODEL },
		{ ".obj",		ResourceType::MODEL },
		{ ".ilargi",	ResourceType::SCENE },
	};

	using ImportFn = std::function<void(const std::filesystem::path&, const std::filesystem::path&)>;
	static std::map<ResourceType, ImportFn> importers =
	{
		{ ResourceType::MESH, ModelImporter::ImportModel2 }
	};

	using LoadFn = std::function<void(const ResourceMetadata&)>;
	//static std::map<ResourceType, LoadFn> loaders =
	//{
	//	{ ResourceType::MESH, ModelImporter::LoadModel }
	//};

	std::unordered_map<UUID, ResourceMetadata> ResourceManager::resourcesMetadata;
	std::unordered_map<UUID, std::shared_ptr<Resource>> ResourceManager::loadedResources;
	
	UUID ResourceManager::RegisterResource(const ResourceMetadata& metadata)
	{
		UUID resourceUUID;
		
		resourcesMetadata[resourceUUID] = metadata;

		return resourceUUID;
	}

	UUID ResourceManager::ImportResource(const std::filesystem::path& path)
	{
		UUID resourceUUID;

		std::string newPath = ("Assets" / path.stem()).string() + ".ires";
		
		ResourceMetadata metadata;
		metadata.type = GetResourceType(path.extension().string());
		metadata.sourceFile = path;
		metadata.filepath = newPath;

		resourcesMetadata[resourceUUID] = metadata;
		importers[metadata.type](path, newPath);

		return resourceUUID;
	}

	bool ResourceManager::ExistsResource(UUID uuid)
	{
		return resourcesMetadata.find(uuid) != resourcesMetadata.end();
	}

	std::shared_ptr<Resource> ResourceManager::GetResource(UUID uuid)
	{
		if (!ExistsResource(uuid))
			return nullptr;

		std::shared_ptr<Resource> resource;
		if (IsResourceLoaded(uuid))
		{
			resource = loadedResources.at(uuid);
			return resource;
		}

		const auto& metadata = resourcesMetadata.at(uuid);
		//resource = Importer::Load(uuid, metadata);
		//loadedResources[uuid] = resource;

		return resource;
	}

	bool ResourceManager::IsResourceLoaded(UUID uuid)
	{
		return loadedResources.find(uuid) != loadedResources.end();
	}
	
	const ResourceType ResourceManager::GetResourceType(const std::string& str)
	{
		if (extensionsMap.contains(str))
			return extensionsMap.at(str);

		return ResourceType::NONE;
	}

	void ResourceManager::SaveResourceRegistry()
	{
		JsonDocument document;

		std::ofstream file("ResourceRegistry.json", std::ios::out | std::ios::binary);

		for (auto it = resourcesMetadata.begin(); it != resourcesMetadata.end(); ++it)
		{
			const ResourceMetadata& metadata = it->second;
			int index = document.size();

			document[index]["UUID"] = static_cast<uint64_t>(it->first);
			document[index]["Type"] = static_cast<int>(metadata.type);
			document[index]["SourceFile"] = metadata.sourceFile.string();
			document[index]["Filepath"] = metadata.filepath.string();
		}

		serializeJsonPretty(document, file);

		file.close();
	}
	
	void ResourceManager::LoadResourceRegistry()
	{
		JsonDocument document;

		std::ifstream file("ResourceRegistry.json", std::ios::in | std::ios::binary);
		
		deserializeJson(document, file);
		file.close();
		
		for (int i = 0; i < document.size(); ++i)
		{
			UUID uuid = static_cast<uint64_t>(document[i]["UUID"]);
			ResourceMetadata metadata;

			metadata.type = static_cast<ResourceType>((int)document[i]["Type"]);
			metadata.sourceFile = static_cast<const char*>(document[i]["SourceFile"]);
			metadata.filepath = static_cast<const char*>(document[i]["Filepath"]);

			resourcesMetadata[uuid] = metadata;
		}
	}
}