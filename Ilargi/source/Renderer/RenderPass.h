#pragma once

#include "Framebuffer.h"

namespace Ilargi
{
	class Framebuffer;
	class CommandBuffer;
	class Pipeline;

	struct RenderPassProperties
	{
		std::shared_ptr<Framebuffer> framebuffer;
		std::shared_ptr<Pipeline> pipeline;
		bool clearValues;
	};

	class RenderPass : public std::enable_shared_from_this<RenderPass>
	{
	public:
		virtual void Destroy() = 0;

		virtual void BeginRenderPass(const std::shared_ptr<CommandBuffer>& aCommandBuffer) const = 0;
		virtual void EndRenderPass(const std::shared_ptr<CommandBuffer>& aCommandBuffer) const = 0;

		virtual const RenderPassProperties& GetProperties() const = 0;

		template <typename T>
		std::shared_ptr<T> As()
		{
			ILG_STATIC_ASSERT(std::is_base_of<RenderPass, T>::value, "T must be a derived class of RenderPass");

			return std::static_pointer_cast<T>(shared_from_this());
		}

		static std::shared_ptr<RenderPass> Create(const RenderPassProperties& aProperties);
	};
}