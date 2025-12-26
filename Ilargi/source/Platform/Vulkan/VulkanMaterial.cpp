#include "ilargipch.h"

#include "Renderer/Renderer.h"

#include "VulkanMaterial.h"
#include "VulkanContext.h"
#include "VulkanTexture.h"
#include "VulkanShader.h"

namespace Ilargi
{
	VulkanMaterial::VulkanMaterial(std::shared_ptr<Shader> aShader, const MaterialData& aMaterialData) 
		: mShader(aShader), mDescriptorSet(VK_NULL_HANDLE), mMaterialData(aMaterialData)
	{
		auto vulkanShader{ aShader->As<VulkanShader>() };
		vulkanShader->AllocateDescriptorSet(0, mDescriptorSet);

		mDiffuse = Renderer::GetDefaultTexture();

		auto device{ VulkanContext::GetLogicalDevice() };

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
		VulkanAllocator::UnmapMemory(mMaterialBuffer);
		VulkanAllocator::DestroyBuffer(mMaterialBuffer);
	}
	
	void VulkanMaterial::UpdateDiffuse(std::shared_ptr<Texture2D> aTexture)
	{
		mDiffuse = aTexture;

		UpdateDescriptor();
	}
	
	void VulkanMaterial::UpdateMaterialData()
	{
		UpdateDescriptor();
	}

	void VulkanMaterial::UpdateDescriptor()
	{
		auto device{ VulkanContext::GetLogicalDevice() };
		auto albedo{ std::static_pointer_cast<VulkanTexture2D>(mDiffuse) };

		memcpy(mMaterialBufferMapped, &mMaterialData, sizeof(MaterialData));
		VkDescriptorBufferInfo bufferInfo
		{
			mMaterialBuffer.buffer,
			0,
			sizeof(MaterialData)
		};

		std::vector<VkWriteDescriptorSet> descriptorWrites
		{
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
			}
		};

		if (albedo)
		{
			VkDescriptorImageInfo imageInfo
			{
				albedo->GetSampler(),
				albedo->GetImageView(),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			};

			auto& descriptorWrite{ descriptorWrites.emplace_back() };

			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = mDescriptorSet;
			descriptorWrite.dstBinding = 0;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite.pImageInfo = &imageInfo;
		}

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}