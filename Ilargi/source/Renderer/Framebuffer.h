#pragma once

namespace Ilargi
{
	enum class ImageFormat
	{
		NONE = 0,

		RGBA8,
		RGBA16,
		RGBA32,

		RED8,
		RED16,
		RED32,

		DEPTH32,
		DEPTH24_STENCIL8,
	};

	struct FramebufferProperties
	{
		uint32_t width;
		uint32_t height;
		std::vector<ImageFormat> formats;

		bool swapchainTarget;
	};

	class RenderPass;

	class Framebuffer
	{
	public:
		virtual const FramebufferProperties& GetProperties() const = 0;

		virtual void Destroy() = 0;

		virtual void Resize(const std::shared_ptr<RenderPass>& renderPass, uint32_t width, uint32_t height) = 0;

		virtual void* GetID() const = 0;

		virtual const uint32_t GetWidth() const = 0;
		virtual const uint32_t GetHeight() const = 0;

		virtual const std::vector<ImageFormat>& GetColorSpecifications() const = 0;
		virtual const ImageFormat GetDepthSpecification() const = 0;

		static std::shared_ptr<Framebuffer> Create(const FramebufferProperties& props);
	};
}