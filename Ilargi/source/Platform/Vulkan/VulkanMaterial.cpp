#include "ilargipch.h"

#include "VulkanMaterial.h"
#include "VulkanContext.h"
#include "VulkanTexture.h"
#include "VulkanShader.h"

namespace Ilargi
{
	VulkanMaterial::VulkanMaterial(std::shared_ptr<Shader> shader)
	{
		auto device = VulkanContext::GetLogicalDevice();

		albedo = Texture2D::Create(std::filesystem::path("assets/textures/viking_room.png"));
		
		auto vulkanShader = std::static_pointer_cast<VulkanShader>(shader);
		descriptorSet = vulkanShader->AllocateDescriptorSet();

		auto albedoTexture = std::static_pointer_cast<VulkanTexture2D>(albedo);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = albedoTexture->GetImageView();
		imageInfo.sampler = albedoTexture->GetSampler();

		std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSet;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
	
	VulkanMaterial::~VulkanMaterial()
	{

	}
}