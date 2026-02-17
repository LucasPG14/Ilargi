#include "ilargipch.h"
#include "RenderPassManager.h"

namespace Ilargi
{
	const IRenderPass& RenderPassManager::GetRenderPass(const RenderPassProperties& aRenderPassProperties)
	{
		if (mRenderPasses.find(aRenderPassProperties) == mRenderPasses.end())
			mRenderPasses[aRenderPassProperties] = IRenderPass::Create(aRenderPassProperties);
		
		return *mRenderPasses[aRenderPassProperties];
	}
}