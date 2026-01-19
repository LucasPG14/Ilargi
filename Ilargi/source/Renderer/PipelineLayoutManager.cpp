#include "ilargipch.h"
#include "PipelineLayoutManager.h"

namespace Ilargi
{
	const std::shared_ptr<PipelineLayout>& PipelineLayoutManager::GetPipelineLayout(const PipelineLayoutProperties& aProperties)
	{
		if (mPipelineLayouts.find(aProperties) == mPipelineLayouts.end())
			mPipelineLayouts[aProperties] = PipelineLayout::Create(aProperties);

		return mPipelineLayouts[aProperties];
	}
	
	const std::shared_ptr<DescriptorSetLayout>& PipelineLayoutManager::GetDescriptorSetLayout(const DescriptorSetLayoutProperties& aProperties)
	{
		if (mDescriptorSetLayouts.find(aProperties) == mDescriptorSetLayouts.end())
			mDescriptorSetLayouts[aProperties] = DescriptorSetLayout::Create(aProperties);

		return mDescriptorSetLayouts[aProperties];
	}
}