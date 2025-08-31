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

		static ResourceType GetStaticType() { return ResourceType::MODEL; }
		const ResourceType GetType() const { return GetStaticType(); }

		const std::vector<std::shared_ptr<StaticMesh>> GetMeshes() const { return mMeshes; }
		const std::vector<std::shared_ptr<Material>> GetMaterials() const { return mMaterials; }

	private:
		std::vector<std::shared_ptr<StaticMesh>> mMeshes;
		std::vector<std::shared_ptr<Material>> mMaterials;
	};
}