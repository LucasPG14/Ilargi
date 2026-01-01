#pragma once

#include "Resource.h"

namespace Ilargi
{
	class Texture2D : public Resource
	{
	public:
		/*
		* @brief Returns the texture width.
		* @return The texture width.
		*/
		[[nodiscard]] virtual const uint32_t GetWidth() const = 0;

		/*
		* @brief Returns the texture height.
		* @return The texture height.
		*/
		[[nodiscard]] virtual const uint32_t GetHeight() const = 0;

		/*
		* @brief Returns the texture ID.
		* @return The texture ID.
		*/
		[[nodiscard]] virtual const void* GetID() const = 0;

		/*
		* @brief Returns the resource type.
		* @return The resource type.
		*/
		[[nodiscard]] static ResourceType GetStaticType() { return ResourceType::TEXTURE2D; }

		/*
		* @copydoc Resource::GetType()
		*/
		[[nodiscard]] const ResourceType GetType() const { return GetStaticType(); }

		/*
		* @brief Creates the texture 2D.
		* @param aFilepath The filepath of the texture.
		* @return An instance of the texture 2D created.
		*/
		static std::shared_ptr<Texture2D> Create(std::filesystem::path aFilepath);

		/*
		* @brief Creates the texture 2D.
		* @param aData The texture data.
		* @param aWidth The texture width.
		* @param aHeight The texture height.
		* @param aChannels The number of channels of the texture.
		* @return An instance of the texture 2D created.
		*/
		static std::shared_ptr<Texture2D> Create(void* aData, int aWidth, int aHeight, int aChannels);
	};
}