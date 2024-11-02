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
		VulkanMaterial(std::shared_ptr<Shader> aShader);
		virtual ~VulkanMaterial();

		const void* GetDescriptorSet() const override { return mDescriptorSet; }
		const MaterialData& GetMaterialData() const override { return mMaterialData; }
		MaterialData& GetMaterialData() override { return mMaterialData; }
		std::shared_ptr<Texture2D> GetDiffuse() override { return mDiffuse; }

		void SetDiffuse(std::shared_ptr<Texture2D> aTexture) override;

	private:
		void UpdateDescriptor();

	private:
		VkDescriptorSet mDescriptorSet;

		MaterialData mMaterialData;
		VulkanBuffer mMaterialBuffer;
		void* mMaterialBufferMapped;
		std::shared_ptr<Texture2D> mDiffuse;
	};
}