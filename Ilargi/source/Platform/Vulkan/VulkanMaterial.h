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
		VulkanMaterial(std::shared_ptr<Shader> aShader, const MaterialData& aMaterialData);
		virtual ~VulkanMaterial();

		const std::shared_ptr<Shader>& GetShader() const override { return mShader; }

		[[nodiscard]] const void* GetDescriptorSet() const override { return mDescriptorSet; }
		[[nodiscard]] const MaterialData& GetMaterialData() const override { return mMaterialData; }
		[[nodiscard]] MaterialData& GetMaterialData() override { return mMaterialData; }
		[[nodiscard]] std::shared_ptr<Texture2D> GetDiffuse() override { return mDiffuse; }

		void UpdateDiffuse(std::shared_ptr<Texture2D> aTexture) override;
		void UpdateMaterialData() override;

	private:
		void UpdateDescriptor();

	private:
		std::shared_ptr<Shader> mShader;
		VkDescriptorSet mDescriptorSet;

		MaterialData mMaterialData;
		VulkanBuffer mMaterialBuffer;
		void* mMaterialBufferMapped;
		std::shared_ptr<Texture2D> mDiffuse;
	};
}