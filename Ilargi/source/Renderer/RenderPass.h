#pragma once

namespace Ilargi
{
	class Framebuffer;
	class CommandBuffer;

	struct RenderPassProperties
	{
		std::shared_ptr<Framebuffer> framebuffer;
	};

	class RenderPass
	{
	public:
		virtual void Destroy() = 0;

		virtual void BeginRenderPass(std::shared_ptr<CommandBuffer> commandBuffer) const = 0;

		virtual const RenderPassProperties& GetProperties() const = 0;

		static std::shared_ptr<RenderPass> Create(const RenderPassProperties& props);
	};
}