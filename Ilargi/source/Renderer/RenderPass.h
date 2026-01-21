#pragma once

#include "Framebuffer.h"

namespace Ilargi
{
	class Framebuffer;
	class CommandBuffer;
	class Pipeline;

	struct RenderPassProperties
	{
		std::vector<ImageFormat> Formats;
		bool ClearValues;

		bool operator==(const RenderPassProperties& aProperties) const
		{
			return Formats == aProperties.Formats;
		}
	};

	class RenderPass : public std::enable_shared_from_this<RenderPass>
	{
	public:
		/*
		* @brief Destroys the render pass data.
		*/
		virtual void Destroy() = 0;

		/*
		* @brief Begins the render pass.
		* @param aCommandBuffer The command buffer used to execute the render pass action.
		* @param aFramebuffer The framebuffer used to draw.
		*/
		virtual void BeginRenderPass(const std::shared_ptr<CommandBuffer>& aCommandBuffer, const std::shared_ptr<Framebuffer>& aFramebuffer) const = 0;
		
		/*
		* @brief Ends the render pass.
		* @param aCommandBuffer The command buffer used to execute the render pass action.
		*/
		virtual void EndRenderPass(const std::shared_ptr<CommandBuffer>& aCommandBuffer) const = 0;

		/*
		* @brief Returns the render pass properties.
		* @return The render pass properties.
		*/
		[[nodiscard]] virtual const RenderPassProperties& GetProperties() const = 0;

		/*
		* @brief Casts the render pass to the specified template class.
		* @tparam The destination type to which the render pass will be cast.
		* @return An instance of type 'T' created from the render pass.
		*/
		template <typename T>
		std::shared_ptr<T> As()
		{
			ILG_STATIC_ASSERT(std::is_base_of<RenderPass, T>::value, "T must be a derived class of RenderPass");

			return std::static_pointer_cast<T>(shared_from_this());
		}

		/*
		* @brief Creates the render pass.
		* @param aProperties The properties of the render pass that will be created.
		* @return An instance of the render pass created.
		*/
		static std::shared_ptr<RenderPass> Create(const RenderPassProperties& aProperties);
	};
}

template<>
struct std::hash<Ilargi::RenderPassProperties>
{
	size_t operator()(const Ilargi::RenderPassProperties& aProperties) const
	{
		size_t h{ 0 };
		for (const auto& format : aProperties.Formats)
			h ^= std::hash<int>{}(static_cast<int>(format));

		h ^= std::hash<bool>{}(aProperties.ClearValues) << 1;
		return h;
	}
};