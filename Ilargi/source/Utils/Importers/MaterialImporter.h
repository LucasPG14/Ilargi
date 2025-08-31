#pragma once

#include "Resources/Resource.h"

namespace Ilargi
{
	class MaterialImporter
	{
	public:
		static std::shared_ptr<Resource> LoadMaterial(const ResourceMetadata& aMetadata);
	};
}