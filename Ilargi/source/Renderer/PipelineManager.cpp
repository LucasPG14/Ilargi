#include "ilargipch.h"
#include "PipelineManager.h"

namespace Ilargi
{
	PipelineManager::PipelineManager()
	{
	}
	
	const std::shared_ptr<IGraphicsPipeline>& PipelineManager::GetPipeline(const GraphicsPipelineProperties& aPipelineProperties)
	{
		if (mPipelines.find(aPipelineProperties) == mPipelines.end())
			mPipelines[aPipelineProperties] = IGraphicsPipeline::Create(aPipelineProperties);

		return mPipelines[aPipelineProperties];
	}
}