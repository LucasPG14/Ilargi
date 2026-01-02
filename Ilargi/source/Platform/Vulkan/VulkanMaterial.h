#pragma once

#include "Resources/Material.h"
#include "VulkanAllocator.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class Texture2D;

	class VulkanMaterial : public Material
	{
	public:
		/*
		* @brief Constructor.
		* @param aShader Instance of the shader used by the material.
		* @param aMaterialData The material data.
		*/
		VulkanMaterial(const std::shared_ptr<Shader>& aShader, const MaterialData& aMaterialData);
		
		/*
		* @brief Destructor.
		*/
		virtual ~VulkanMaterial();

		/*
		* @copydoc Material::GetShader().
		*/
		const std::shared_ptr<Shader>& GetShader() const override { return mShader; }

		/*
		* @brief Returns the material descriptor set.
		*/
		[[nodiscard]] const VkDescriptorSet GetDescriptorSet() const { return mDescriptorSet; }

		/*
		* @copydoc Material::GetMaterialData().
		*/
		[[nodiscard]] const MaterialData& GetMaterialData() const override { return mMaterialData; }

		/*
		* @copydoc Material::GetMaterialData().
		*/
		[[nodiscard]] MaterialData& GetMaterialData() override { return mMaterialData; }

		/*
		* @copydoc Material::GetDiffuse().
		*/
		[[nodiscard]] std::shared_ptr<Texture2D> GetDiffuse() override { return mDiffuse; }

		/*
		* @copydoc Material::UpdateDiffuse().
		*/
		void UpdateDiffuse(std::shared_ptr<Texture2D> aTexture) override;

		/*
		* @copydoc Material::UpdateMaterialData().
		*/
		void UpdateMaterialData() override;

	private:
		/*
		* @brief Update the descriptor data for vulkan.
		*/
		void UpdateDescriptor();

	private:
		std::shared_ptr<Shader> mShader; // Instance of the shader used by the material.
		std::shared_ptr<Texture2D> mDiffuse; // Instance of the diffuse texture used by the material.
		VkDescriptorSet mDescriptorSet; // The ID of the material.
		VulkanBuffer mMaterialBuffer; // The Vulkan buffer and allocation of the material.
		void* mMaterialBufferMapped; // The pointer to the data stored on the descriptor set.
		MaterialData mMaterialData; // The material data(color, metallic factor...)
	};
}