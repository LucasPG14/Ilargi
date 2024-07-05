#pragma once

#include "Resources/Material.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class Texture2D;

	class VulkanMaterial : public Material
	{
	public:
		VulkanMaterial(std::shared_ptr<Shader> shader);
		virtual ~VulkanMaterial();

		const void* GetDescriptorSet() const override { return descriptorSet; }
		std::shared_ptr<Texture2D> GetDiffuse() override { return diffuse; }

		void SetDiffuse(std::shared_ptr<Texture2D> texture) override;

	private:
		void UpdateDescriptor();

	private:
		VkDescriptorSet descriptorSet;

		std::shared_ptr<Texture2D> diffuse;
	};
}