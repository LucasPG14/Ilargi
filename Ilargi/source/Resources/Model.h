#pragma once

#include "Resource.h"

namespace Ilargi
{
	class StaticMesh;
	class Material;

	class Model : public Resource
	{
	public:
		Model(const std::vector<std::shared_ptr<StaticMesh>>& aMeshes, const std::vector<std::shared_ptr<Material>>& aMaterials);
		virtual ~Model();

		[[nodiscard]] static ResourceType GetStaticType() { return ResourceType::MODEL; }
		[[nodiscard]] const ResourceType GetType() const { return GetStaticType(); }

		[[nodiscard]] const std::vector<std::shared_ptr<StaticMesh>>& GetMeshes() const { return mMeshes; }
		[[nodiscard]] const std::vector<std::shared_ptr<Material>>& GetMaterials() const { return mMaterials; }

	private:
		std::vector<std::shared_ptr<StaticMesh>> mMeshes;
		std::vector<std::shared_ptr<Material>> mMaterials;
	};
}