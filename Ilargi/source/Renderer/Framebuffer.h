#pragma once

namespace Ilargi
{
	struct FramebufferProperties
	{
		uint32_t width;
		uint32_t height;
	};

	class Framebuffer
	{
	public:
		virtual const FramebufferProperties& GetProperties() const = 0;

		static std::shared_ptr<Framebuffer> Create(const FramebufferProperties& props);
	};
}