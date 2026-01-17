#include "ilargipch.h"
#include "ThumbnailManager.h"

#include "TextureThumbnail.h"

#include "Resources/ResourceManager.h"

namespace Ilargi
{
	std::unordered_map<UUID, std::shared_ptr<Texture2D>> ThumbnailManager::sThumbnails{};

	void ThumbnailManager::Init()
	{
		if (!std::filesystem::exists("Engine/Thumbnails"))
			std::filesystem::create_directory("Engine/Thumbnails");
	}

	void ThumbnailManager::Clear()
	{
		sThumbnails.clear();
	}

	std::shared_ptr<Texture2D> ThumbnailManager::GetThumbnail(UUID aUUID)
	{
		if (sThumbnails.find(aUUID) != sThumbnails.end())
			return sThumbnails[aUUID];

		sThumbnails[aUUID] = CreateThumbnail(aUUID);

		return sThumbnails[aUUID];
	}

	std::shared_ptr<Texture2D> ThumbnailManager::CreateThumbnail(UUID aUUID)
	{
		const ResourceMetadata& metadata{ResourceManager::GetMetadata(aUUID)};
		switch (metadata.type)
		{
		case ResourceType::TEXTURE2D: return TextureThumbnail::Create(aUUID, metadata);
		}

		return nullptr;
	}
}