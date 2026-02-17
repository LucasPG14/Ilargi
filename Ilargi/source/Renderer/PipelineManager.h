#pragma once

#include "IGraphicsPipeline.h"

namespace Ilargi
{
	class PipelineManager
	{
	public:
		/*
		* @brief Constructor.
		*/
		PipelineManager();

		/*
		* @brief Get or create a pipeline given the specified properties.
		* @param aProperties The properties of the pipeline.
		* @return An instance of the pipeline with the given properties.
		*/
		const std::shared_ptr<IGraphicsPipeline>& GetPipeline(const GraphicsPipelineProperties& aPipelineProperties);

	private:
		std::unordered_map<GraphicsPipelineProperties, std::shared_ptr<IGraphicsPipeline>> mPipelines; // Container of the pipelines.
	};
}