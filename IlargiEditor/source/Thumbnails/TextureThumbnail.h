#pragma once

#include "Base/UUID.h"

namespace Ilargi
{
	class Texture2D;
	struct ResourceMetadata;

	class TextureThumbnail
	{
	public:
		/*
		* @brief Creates the texture thumbnail.
		* @param aUUID The identifier of the resource.
		* @param aMetadata The metadata of the resource.
		* @return Pointer of the thumbnail created.
		*/
		static std::shared_ptr<Texture2D> Create(UUID aUUID, const ResourceMetadata& aMetadata);
	};
}