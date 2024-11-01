#include "ilargipch.h"

#include "Renderer/Renderer.h"

#include "VulkanMaterial.h"
#include "VulkanContext.h"
#include "VulkanTexture.h"
#include "VulkanShader.h"

namespace Ilargi
{
	VulkanMaterial::VulkanMaterial(std::shared_ptr<Shader> aShader) : mDescriptorSet(VK_NULL_HANDLE)
	{
		auto vulkanShader = std::static_pointer_cast<VulkanShader>(aShader);
		vulkanShader->AllocateDescriptorSet(0, mDescriptorSet);

		mDiffuse = Renderer::GetDefaultTexture();
		UpdateDescriptor();
	}
	
	VulkanMaterial::~VulkanMaterial()
	{

	}
	
	void VulkanMaterial::SetDiffuse(std::shared_ptr<Texture2D> aTexture)
	{
		mDiffuse = aTexture;

		UpdateDescriptor();
	}
	
	void VulkanMaterial::UpdateDescriptor()
	{
		auto device = VulkanContext::GetLogicalDevice();

		auto albedoTexture = std::static_pointer_cast<VulkanTexture2D>(mDiffuse);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = albedoTexture->GetImageView();
		imageInfo.sampler = albedoTexture->GetSampler();

		std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = mDescriptorSet;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}