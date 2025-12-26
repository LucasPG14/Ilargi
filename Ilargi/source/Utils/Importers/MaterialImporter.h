#pragma once

#include "Resources/Resource.h"

namespace Ilargi
{
	class Material;

	class MaterialImporter
	{
	public:
		static std::shared_ptr<Resource> LoadMaterial(const ResourceMetadata& aMetadata);
		static void SaveMaterial(const ResourceMetadata& aMetadata, const std::shared_ptr<Resource>& aResource);
	};
}