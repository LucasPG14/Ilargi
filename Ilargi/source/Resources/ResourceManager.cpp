#include "ilargipch.h"

#include "ResourceManager.h"
#include "Utils/Importers/ModelImporter.h"
#include "Utils/Importers/TextureImporter.h"
#include "Utils/Importers/SceneImporter.h"

#include <ArduinoJson-v7.0.4.h>

namespace Ilargi
{
	namespace Utils
	{
		std::string GetExtensionFromResourceType(ResourceType type)
		{
			switch (type)
			{
			case ResourceType::TEXTURE2D: return ".itex";
			case ResourceType::MODEL: return ".imodel";
			case ResourceType::SCENE: return ".ilargi";
			}
		}
	}

	const std::map<std::string, ResourceType> extensionsMap = 
	{
		{ ".png",		ResourceType::TEXTURE2D },
		{ ".jpg",		ResourceType::TEXTURE2D },
		{ ".jpeg",		ResourceType::TEXTURE2D },
		{ ".fbx",		ResourceType::MODEL },
		{ ".obj",		ResourceType::MODEL },
		{ ".ilargi",	ResourceType::SCENE },
	};

	using ImportFn = std::function<void(UUID, const ResourceMetadata&)>;
	static std::map<ResourceType, ImportFn> importers =
	{
		{ ResourceType::MODEL, ModelImporter::ImportModel },
		{ ResourceType::TEXTURE2D, TextureImporter::ImportTexture },
		{ ResourceType::SCENE, SceneImporter::ImportScene },
	};

	using LoadFn = std::function<std::shared_ptr<Resource>(const ResourceMetadata&)>;
	static std::map<ResourceType, LoadFn> loaders =
	{
		{ ResourceType::TEXTURE2D, TextureImporter::LoadTexture },
		{ ResourceType::MODEL, ModelImporter::LoadModel },
		{ ResourceType::SCENE, SceneImporter::LoadScene },
	};

	std::unordered_map<UUID, ResourceMetadata> ResourceManager::resourcesMetadata;
	std::unordered_map<UUID, std::shared_ptr<Resource>> ResourceManager::loadedResources;
	
	void ResourceManager::Clear()
	{
		resourcesMetadata.clear();
		loadedResources.clear();
	}

	UUID ResourceManager::RegisterResource(const ResourceMetadata& metadata)
	{
		UUID resourceUUID;
		
		resourcesMetadata[resourceUUID] = metadata;

		return resourceUUID;
	}

	UUID ResourceManager::ImportResource(const std::filesystem::path& actualDir, const std::filesystem::path& path)
	{
		UUID resourceUUID;
		
		ResourceMetadata metadata;
		metadata.type = GetResourceType(path.extension().string());
		
		std::string newPath = (actualDir / path.stem()).string() + Utils::GetExtensionFromResourceType(metadata.type);
		
		metadata.sourceFile = path;
		metadata.filepath = newPath;

		resourcesMetadata[resourceUUID] = metadata;
		importers[metadata.type](resourceUUID, metadata);

		return resourceUUID;
	}

	bool ResourceManager::ExistsResource(UUID uuid)
	{
		return resourcesMetadata.find(uuid) != resourcesMetadata.end();
	}

	const ResourceMetadata& ResourceManager::GetMetadata(UUID uuid)
	{
		if (ExistsResource(uuid))
			return resourcesMetadata[uuid];

		return ResourceMetadata();
	}

	std::shared_ptr<Resource> ResourceManager::GetResource(UUID uuid)
	{
		if (!ExistsResource(uuid))
			return nullptr;

		std::shared_ptr<Resource> resource = nullptr;
		if (IsResourceLoaded(uuid))
		{
			resource = loadedResources.at(uuid);
			return resource;
		}

		const auto& metadata = resourcesMetadata.at(uuid);
		resource = loaders[metadata.type](metadata);
		resource->resourceUUID = uuid;
		loadedResources[uuid] = resource;

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
		file.clear();

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

			metadata.filepath = static_cast<const char*>(document[i]["Filepath"]);
			
			//if (!std::filesystem::exists(metadata.filepath))
			//	continue;

			metadata.type = static_cast<ResourceType>((int)document[i]["Type"]);
			metadata.sourceFile = static_cast<const char*>(document[i]["SourceFile"]);

			resourcesMetadata[uuid] = metadata;
		}
	}
}