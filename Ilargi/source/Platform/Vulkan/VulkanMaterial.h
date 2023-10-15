#pragma once

#include "Renderer/Material.h"
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

	private:
		VkDescriptorSet descriptorSet;

		std::shared_ptr<Texture2D> albedo;
	};
}