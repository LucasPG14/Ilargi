#include "ilargipch.h"
#include "PipelineLayoutManager.h"

namespace Ilargi
{
	const std::shared_ptr<IPipelineLayout>& PipelineLayoutManager::GetPipelineLayout(const PipelineLayoutProperties& aProperties)
	{
		if (mPipelineLayouts.find(aProperties) == mPipelineLayouts.end())
			mPipelineLayouts[aProperties] = IPipelineLayout::Create(aProperties);

		return mPipelineLayouts[aProperties];
	}
	
	const std::shared_ptr<IDescriptorSetLayout>& PipelineLayoutManager::GetDescriptorSetLayout(const DescriptorSetLayoutProperties& aProperties)
	{
		if (mDescriptorSetLayouts.find(aProperties) == mDescriptorSetLayouts.end())
			mDescriptorSetLayouts[aProperties] = IDescriptorSetLayout::Create(aProperties);

		return mDescriptorSetLayouts[aProperties];
	}
}