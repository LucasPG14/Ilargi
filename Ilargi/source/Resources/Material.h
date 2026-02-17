#pragma once

#include "Resources/Resource.h"
#include "Renderer/IShader.h"

namespace Ilargi
{
	class Texture2D;

	struct MaterialData
	{
		glm::vec4 color{ glm::vec4(1.0f) };
		float metallic{ 0.5f };
		float roughness{ 0.5f };
	};

	class Material : public Resource
	{
	public:
		/*
		* @brief Returns the resource type.
		* @return The resource type.
		*/
		[[nodiscard]] static ResourceType GetStaticType() { return ResourceType::MATERIAL; }

		/*
		* @copydoc Resource::GetType()
		*/
		[[nodiscard]] const ResourceType GetType() const override { return GetStaticType(); }

		/*
		* @brief Returns the shader used by the material.
		* @return An instance of the shader used by the material.
		*/
		[[nodiscard]] virtual const std::shared_ptr<IShader>& GetShader() const = 0;

		/*
		* @brief Returns the material data.
		* @return The material data.
		*/
		[[nodiscard]] virtual const MaterialData& GetMaterialData() const = 0;

		/*
		* @brief Returns the material data.
		* @return The material data.
		*/
		[[nodiscard]] virtual MaterialData& GetMaterialData() = 0;

		/*
		* @brief Returns the texture specified.
		* @param aTextureName The texture name.
		* @return An instance of the texture.
		*/
		[[nodiscard]] virtual const std::shared_ptr<Texture2D>& GetTexture(const std::string& aTextureName) = 0;

		/*
		* @brief Returns all the textures.
		* @return The map of textures.
		*/
		[[nodiscard]] virtual const std::map<std::string, std::shared_ptr<Texture2D>>& GetTextures() = 0;

		/*
		* @brief Updates the specified texture.
		* @param aTextureName The name of the texture.
		* @param aTexture The new texture.
		*/
		virtual void UpdateTexture(const std::string& aTextureName, const std::shared_ptr<Texture2D>& aTexture) = 0;

		/*
		* @brief Updates the material data.
		*/
		virtual void UpdateMaterialData() = 0;

		/*
		* @brief Creates the material.
		* @param aShader Instance of the shader used by the material.
		* @param aMaterialData The material data.
		* @return An instance of the material created.
		*/
		static std::shared_ptr<Material> Create(const std::shared_ptr<IShader>& aShader, const MaterialData& aMaterialData = {});
	};
}