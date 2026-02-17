#pragma once

#include "IFramebuffer.h"

namespace Ilargi
{
	struct RenderPassProperties
	{
		std::vector<ImageFormat> Formats;

		bool operator==(const RenderPassProperties& aProperties) const
		{
			return Formats == aProperties.Formats;
		}
	};

	class IRenderPass : public std::enable_shared_from_this<IRenderPass>
	{
	public:
		/*
		* @brief Destroys the render pass data.
		*/
		virtual void Destroy() = 0;

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
		const T& As() const
		{
			ILG_STATIC_ASSERT(std::is_base_of<IRenderPass, T>::value, "T must be a derived class of IRenderPass");

			return static_cast<const T&>(*this);
		}

		/*
		* @brief Creates the render pass.
		* @param aProperties The properties of the render pass that will be created.
		* @return An instance of the render pass created.
		*/
		static std::unique_ptr<IRenderPass> Create(const RenderPassProperties& aProperties);
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

		return h;
	}
};