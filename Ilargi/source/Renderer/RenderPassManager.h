#pragma once

#include "IRenderPass.h"

namespace Ilargi
{
	class RenderPassManager
	{
	public:
		/*
		* @brief Constructor.
		*/
		RenderPassManager() {}

		/*
		* @brief Get or create a render pass given the specified properties.
		* @param aProperties The properties of the render pass.
		* @return An instance of the render pass with the given properties.
		*/
		const IRenderPass& GetRenderPass(const RenderPassProperties& aProperties);
	
	private:
		std::unordered_map<RenderPassProperties, std::unique_ptr<IRenderPass>> mRenderPasses; // Container of render passes.
	};
}