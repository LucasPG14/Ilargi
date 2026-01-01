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
		/*
		* @brief Constructor.
		* @param aModelNodes The vector of model nodes.
		*/
		Model(const std::vector<ModelNode>& aModelNodes);
		
		/*
		* @brief Destructor.
		*/
		virtual ~Model();

		/*
		* @brief Returns the resource type.
		* @return The resource type.
		*/
		[[nodiscard]] static ResourceType GetStaticType() { return ResourceType::MODEL; }
		
		/*
		* @copydoc Resource::GetType()
		*/
		[[nodiscard]] const ResourceType GetType() const { return GetStaticType(); }

		/*
		* @brief Returns the container of model nodes.
		* @return The model nodes container.
		*/
		[[nodiscard]] const std::vector<ModelNode>& GetModelNodes() const { return mModelNodes; }

	private:
		std::vector<ModelNode> mModelNodes; // Container of the model nodes
	};
}