#pragma once

#include "Renderer/IFramebuffer.h"
#include "Platform/Vulkan/VulkanAllocator.h"

#include <vulkan/vulkan.h>

namespace Ilargi
{
	struct VulkanAttachment
	{
		Image image;
		VkImageView imageView;
	};

	class VulkanFramebuffer : public IFramebuffer
	{
	public:
		/*
		* @brief Constructor.
		* @param aProperties The framebuffer properties.
		*/
		VulkanFramebuffer(const FramebufferProperties& aProperties);

		/*
		* @brief Destructor.
		*/
		virtual ~VulkanFramebuffer();

		/*
		* @brief Initializes the framebuffer.
		*/
		void Init();

		/*
		* @copydoc Framebuffer::Destroy()
		*/
		void Destroy() override;

		/*
		* @copydoc Framebuffer::Resize()
		*/
		void Resize(uint32_t aWidth, uint32_t aHeight) override;

		/*
		* @copydoc Framebuffer::ReadFramebufferPixel()
		*/
		virtual uint32_t ReadFramebufferPixel(uint32_t aX, uint32_t aY) override;

		/*
		* @copydoc Framebuffer::GetProperties()
		*/
		[[nodiscard]] const FramebufferProperties& GetProperties() const override { return mProperties; }
		
		/*
		* @brief Returns the instance of the vulkan framebuffer.
		* @return The instance of the vulkan framebuffer.
		*/
		[[nodiscard]] const VkFramebuffer GetFramebuffer() const { return mFramebuffer; }

		/*
		* @copydoc Framebuffer::GetColorSpecifications()
		*/
		[[nodiscard]] const std::vector<ImageFormat>& GetColorSpecifications() const override { return mColorSpecifications; }
		
		/*
		* @copydoc Framebuffer::GetDepthSpecification()
		*/
		[[nodiscard]] const ImageFormat GetDepthSpecification() const override { return mDepthSpecification; }

		/*
		* @copydoc Framebuffer::GetWidth()
		*/
		[[nodiscard]] const uint32_t GetWidth() const override { return mProperties.Width; }

		/*
		* @copydoc Framebuffer::GetHeight()
		*/
		[[nodiscard]] const uint32_t GetHeight() const override { return mProperties.Height; }

		/*
		* @copydoc Framebuffer::GetID()
		*/
		[[nodiscard]] void* GetID() const override;

	private:
		std::vector<ImageFormat> mColorSpecifications; // Container of the color specifications of the framebuffer.
		std::vector<VulkanAttachment> mColorAttachments; // Container of the color attachments of the framebuffer.

		ImageFormat mDepthSpecification; // Depth specification of the framebuffer.
		VulkanAttachment mDepthAttachment; // Depth attachment of the framebuffer.

		VkFramebuffer mFramebuffer; // Instance of the vulkan framebuffer.
		VkSampler mSampler; // Instance of the vulkan sampler of the framebuffer.
		VkDescriptorSetLayout mDescriptorSetLayout; // Instance of the descriptor set layout for the framebuffer.
		VkDescriptorSet mDescriptorSet; // Instance of the descriptor set for the framebuffer.

		FramebufferProperties mProperties; // The framebuffer properties.
	};
}