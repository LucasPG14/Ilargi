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

		VkBufferCreateInfo bufferInfo
		{
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,	// sType
			nullptr,								// pNext
			0,										// flags
			sizeof(MaterialData),					// size
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,		// usage
			VK_SHARING_MODE_EXCLUSIVE,				// sharingMode
			0,										// queueFamilyIndexCount
			nullptr									// pQueueFamilyIndices
		};

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

		std::array<VkWriteDescriptorSet, 1> descriptorWrites
		{
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, // sType
			nullptr,								// pNext
			mDescriptorSet,							// dstSet
			4,										// dstBinding
			0,										// dstArrayElement
			1,										// descriptorCount
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,		// descriptorType
			nullptr,								// pImageInfo
			&bufferInfo,							// pBufferInfo
			nullptr									// pTexelBufferView
		};

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}