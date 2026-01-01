#pragma once

#include "Resources/Resource.h"

namespace Ilargi
{
	class Shader;
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
		[[nodiscard]] virtual const std::shared_ptr<Shader>& GetShader() const = 0;
		
		/*
		* @brief Returns the id of the material.
		* @return The material id.
		*/
		[[nodiscard]] virtual const void* GetDescriptorSet() const = 0;

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
		* @brief Returns the diffuse texture.
		* @return The diffuse texture.
		*/
		[[nodiscard]] virtual std::shared_ptr<Texture2D> GetDiffuse() = 0;

		/*
		* @brief Updates the diffuse texture.
		* @param aTexture The new diffuse texture.
		*/
		virtual void UpdateDiffuse(std::shared_ptr<Texture2D> aTexture) = 0;

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
		static std::shared_ptr<Material> Create(const std::shared_ptr<Shader>& aShader, const MaterialData& aMaterialData = {});
	};
}