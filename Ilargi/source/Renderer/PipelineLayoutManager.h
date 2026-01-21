#pragma once

#include "PipelineLayout.h"
#include "DescriptorSetLayout.h"

namespace Ilargi
{
	class PipelineLayoutManager
	{
	public:
		/*
		* @brief Constructor.
		*/
		PipelineLayoutManager() {}

		/*
		* @brief Get or create a pipeline layout given the specified properties.
		* @param aProperties The properties of the pipeline layout.
		* @return An instance of the pipeline layout with the given properties.
		*/
		const std::shared_ptr<PipelineLayout>& GetPipelineLayout(const PipelineLayoutProperties& aProperties);
		
		/*
		* @brief Get or create a descriptor set layout given the specified properties.
		* @param aProperties The properties of the descriptor set layout.
		* @return An instance of the descriptor set layout with the given properties.
		*/
		const std::shared_ptr<DescriptorSetLayout>& GetDescriptorSetLayout(const DescriptorSetLayoutProperties& aProperties);

	private:
		std::unordered_map<DescriptorSetLayoutProperties, std::shared_ptr<DescriptorSetLayout>> mDescriptorSetLayouts; // Container of the descriptor set layouts.
		std::unordered_map<PipelineLayoutProperties, std::shared_ptr<PipelineLayout>> mPipelineLayouts; // Container of the pipeline layouts.
	};
}