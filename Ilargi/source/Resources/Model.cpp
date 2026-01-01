#include "ilargipch.h"

#include "Model.h"
#include "Material.h"

#include "Renderer/Renderer.h"

namespace Ilargi
{
	Model::Model(const std::vector<ModelNode>& aModelNodes)
		: mModelNodes(aModelNodes)
	{
	}

	Model::~Model()
	{
	}
}