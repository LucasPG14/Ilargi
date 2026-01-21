#include "ilargipch.h"
#include "RenderPassManager.h"

namespace Ilargi
{
	const std::shared_ptr<RenderPass>& RenderPassManager::GetRenderPass(const RenderPassProperties& aRenderPassProperties)
	{
		if (mRenderPasses.find(aRenderPassProperties) == mRenderPasses.end())
			mRenderPasses[aRenderPassProperties] = RenderPass::Create(aRenderPassProperties);
		
		return mRenderPasses[aRenderPassProperties];
	}
}