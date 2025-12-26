#include "ilargipch.h"

#include "Model.h"
#include "Material.h"

#include "Renderer/Renderer.h"

namespace Ilargi
{
	Model::Model(const std::vector<std::shared_ptr<StaticMesh>>& aMeshes, const std::vector<std::shared_ptr<Material>>& aMaterials) 
		: mMeshes(aMeshes), mMaterials(aMaterials)
	{
		//mMaterials.push_back(Material::Create(Renderer::GetShaderLibrary()->Get("PBR_Static")));
	}

	Model::~Model()
	{
	}
}