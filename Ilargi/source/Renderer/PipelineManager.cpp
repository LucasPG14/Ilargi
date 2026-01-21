#include "ilargipch.h"
#include "PipelineManager.h"

namespace Ilargi
{
	PipelineManager::PipelineManager()
	{
	}
	
	const std::shared_ptr<Pipeline>& PipelineManager::GetPipeline(const PipelineProperties& aPipelineProperties)
	{
		if (mPipelines.find(aPipelineProperties) == mPipelines.end())
			mPipelines[aPipelineProperties] = Pipeline::Create(aPipelineProperties);

		return mPipelines[aPipelineProperties];
	}
}