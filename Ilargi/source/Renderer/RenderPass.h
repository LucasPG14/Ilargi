#pragma once

namespace Ilargi
{
	class Framebuffer;
	class CommandBuffer;
	class Pipeline;

	struct RenderPassProperties
	{
		std::shared_ptr<Framebuffer> framebuffer;
		std::shared_ptr<Pipeline> pipeline;
	};

	class RenderPass
	{
	public:
		virtual void Destroy() = 0;

		virtual void BeginRenderPass(std::shared_ptr<CommandBuffer> commandBuffer) const = 0;
		virtual void EndRenderPass(std::shared_ptr<CommandBuffer> commandBuffer) const = 0;

		virtual void PushConstants(std::shared_ptr<CommandBuffer> commandBuffer, void* data) = 0;

		virtual const RenderPassProperties& GetProperties() const = 0;

		static std::shared_ptr<RenderPass> Create(const RenderPassProperties& props);
	};
}