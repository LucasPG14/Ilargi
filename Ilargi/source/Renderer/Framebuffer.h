#pragma once

namespace Ilargi
{
	enum class ImageFormat
	{
		RGBA_8,
		RGBA_16,
		RGBA_32,

		RED_8,
		RED_16,
		RED_32,
	};

	struct FramebufferProperties
	{
		uint32_t width;
		uint32_t height;
		std::vector<ImageFormat> formats;

		bool swapchainTarget;
	};

	class Pipeline;

	class Framebuffer
	{
	public:
		virtual const FramebufferProperties& GetProperties() const = 0;

		virtual void Destroy() = 0;

		virtual void Resize(std::shared_ptr<Pipeline> pipeline, uint32_t width, uint32_t height) = 0;

		virtual void* GetID() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		static std::shared_ptr<Framebuffer> Create(const FramebufferProperties& props);
	};
}