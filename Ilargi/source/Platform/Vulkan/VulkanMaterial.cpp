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

		auto device = VulkanContext::GetLogicalDevice();

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = sizeof(MaterialData);
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VulkanAllocator::AllocateBuffer(mMaterialBuffer, bufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU);
		mMaterialBufferMapped = VulkanAllocator::MapMemory(mMaterialBuffer);

		UpdateDescriptor();
	}
	
	VulkanMaterial::~VulkanMaterial()
	{
		VulkanAllocator::DestroyBuffer(mMaterialBuffer);
	}
	
	void VulkanMaterial::SetDiffuse(std::shared_ptr<Texture2D> aTexture)
	{
		mDiffuse = aTexture;

		UpdateDescriptor();
	}
	
	void VulkanMaterial::UpdateDescriptor()
	{
		auto device = VulkanContext::GetLogicalDevice();

		memcpy(mMaterialBufferMapped, &mMaterialData, sizeof(MaterialData));

		VkDescriptorBufferInfo bufferInfo
		{
			mMaterialBuffer.buffer,
			0,
			sizeof(MaterialData)
		};

		std::array<VkWriteDescriptorSet, 1> descriptorWrites {};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = mDescriptorSet;
		descriptorWrites[0].dstBinding = 4;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}