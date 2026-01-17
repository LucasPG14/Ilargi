#pragma once

#include "Base/UUID.h"

namespace Ilargi
{
	class Texture2D;

	class ThumbnailManager
	{
	public:
		/*
		* @brief Initializes the Thumbnail manager.
		*/
		static void Init();

		/*
		* @brief Clears the Thumbnail manager data.
		*/
		static void Clear();

		/*
		* @brief Returns the thumbnail of the resource specified.
		* @param aUUID The UUID of the resource.
		* @return Pointer of the thumbnail texture.
		*/
		static std::shared_ptr<Texture2D> GetThumbnail(UUID aUUID);

		/*
		* @brief Creates the thumbnail of the resource specified.
		* @param aUUID The UUID of the resource.
		* @return Pointer of the thumbnail created.
		*/
		static std::shared_ptr<Texture2D> CreateThumbnail(UUID aUUID);
	private:
		static std::unordered_map<UUID, std::shared_ptr<Texture2D>> sThumbnails; // Unordered map of thumbnails.
	};
}