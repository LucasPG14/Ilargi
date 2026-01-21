#pragma once

#include <memory>

namespace Ilargi
{
	class RenderPass;

	enum class ImageFormat
	{
		NONE = 0,

		RGBA8,
		RGBA16,
		RGBA32,

		RED8,
		RED16,
		RED32,
		RED32_UINT,

		DEPTH32,
		DEPTH24_STENCIL8,
	};

	struct FramebufferProperties
	{
		std::vector<ImageFormat> Formats;
		uint16_t Width;
		uint16_t Height;

		bool SwapchainTarget;
		bool MultiSampling;
	};

	class Framebuffer : public std::enable_shared_from_this<Framebuffer>
	{
	public:
		/*
		* @brief Gets the framebuffer properties.
		* @return A reference of the framebuffer properties.
		*/
		virtual const FramebufferProperties& GetProperties() const = 0;

		/*
		* @brief Destroys the framebuffer data.
		*/
		virtual void Destroy() = 0;

		/*
		* @brief Resizes the framebuffer to a given width and height.
		* @param aWidth The new width of the framebuffer.
		* @param aHeight The new height of the framebuffer.
		*/
		virtual void Resize(uint32_t aWidth, uint32_t aHeight) = 0;

		/*
		* @brief Reads a pixel of the framebuffer for a given x and y.
		* @param aX The x position of the pixel.
		* @param aY The y position of the pixel.
		*/
		virtual uint32_t ReadFramebufferPixel(uint32_t aX, uint32_t aY) = 0;

		/*
		* @brief Returns the ID of the framebuffer.
		* @return The ID of the framebuffer.
		*/
		[[nodiscard]] virtual void* GetID() const = 0;

		/*
		* @brief Returns the width of the framebuffer.
		* @return The width of the framebuffer.
		*/
		[[nodiscard]] virtual const uint32_t GetWidth() const = 0;

		/*
		* @brief Returns the height of the framebuffer.
		* @return The height of the framebuffer.
		*/
		[[nodiscard]] virtual const uint32_t GetHeight() const = 0;

		/*
		* @brief Gets a container with the image formats of the color attachments.
		* @return Returns the image formats of the color attachments.
		*/
		[[nodiscard]] virtual const std::vector<ImageFormat>& GetColorSpecifications() const = 0;

		/*
		* @brief Gets the image format of the depth attachment.
		* @return Returns the image format of the depth attachment.
		*/
		[[nodiscard]] virtual const ImageFormat GetDepthSpecification() const = 0;

		/*
		* @brief Casts the framebuffer to the specified template class.
		* @tparam The destination type to which the framebuffer will be cast.
		* @return An instance of type 'T' created from the framebuffer.
		*/
		template <typename T>
		std::shared_ptr<T> As()
		{
			ILG_STATIC_ASSERT(std::is_base_of<Framebuffer, T>::value, "T must be a derived class of Framebuffer");

			return std::static_pointer_cast<T>(shared_from_this());
		}

		/*
		* @brief Creates the framebuffer.
		* @param aProperties The data needed to create the framebuffer.
		* @return An instance of the framebuffer created.
		*/
		static std::shared_ptr<Framebuffer> Create(const FramebufferProperties& aProperties);
	};
}