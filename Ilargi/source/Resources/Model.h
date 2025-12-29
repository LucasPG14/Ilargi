#pragma once

#include "Resource.h"

namespace Ilargi
{
	struct ModelNode
	{
		std::string name;
		glm::mat4 localTransform;

		UUID mesh;
		UUID material;

		std::vector<uint32_t> childrens;
	};

	class StaticMesh;
	class Material;

	class Model : public Resource
	{
	public:
		Model(const std::vector<ModelNode>& aModelNodes);
		virtual ~Model();

		[[nodiscard]] static ResourceType GetStaticType() { return ResourceType::MODEL; }
		[[nodiscard]] const ResourceType GetType() const { return GetStaticType(); }

		[[nodiscard]] const std::vector<ModelNode>& GetModelNodes() const { return nodes; }

	private:
		std::vector<ModelNode> nodes;
	};
}