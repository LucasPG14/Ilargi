#include "ilargipch.h"

#include "Renderer/Renderer.h"

#include "VulkanMaterial.h"
#include "VulkanContext.h"
#include "VulkanTexture.h"
#include "VulkanShader.h"

namespace Ilargi
{
	VulkanMaterial::VulkanMaterial(const std::shared_ptr<Shader>& aShader, const MaterialData& aMaterialData) 
		: mShader(aShader->As<VulkanShader>()), mDescriptorSet(VK_NULL_HANDLE), mMaterialData(aMaterialData)
	{
		auto vulkanShader{ aShader->As<VulkanShader>() };
		vulkanShader->AllocateDescriptorSet(0, mDescriptorSet);

		mBindings = mShader->GetBindings();

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
	
	const std::shared_ptr<Texture2D>& VulkanMaterial::GetTexture(const std::string& aTextureName)
	{
		return mTextures.find(aTextureName) != mTextures.end() ? mTextures[aTextureName] : nullptr;
	}

	void VulkanMaterial::UpdateTexture(const std::string& aTextureName, const std::shared_ptr<Texture2D>& aTexture)
	{
		if (mBindings.find(aTextureName) != mBindings.end())
		{
			mTextures[aTextureName] = aTexture;
			UpdateDescriptor();
		}
	}
	
	void VulkanMaterial::UpdateMaterialData()
	{
		UpdateDescriptor();
	}

	void VulkanMaterial::UpdateDescriptor()
	{
		auto device{ VulkanContext::GetLogicalDevice() };

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

		//std::vector<VkDescriptorImageInfo> imageInfos;

		//for (const auto& [uniformName, texture] : mTextures)
		//{
		//	std::shared_ptr<VulkanTexture2D> vkTexture{ texture->As<VulkanTexture2D>() };
		//	VkDescriptorImageInfo& imageInfo{ imageInfos.emplace_back() };
		//	imageInfo.sampler = vkTexture->GetSampler();
		//	imageInfo.imageView = vkTexture->GetImageView();
		//	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		//	auto& descriptorWrite{ descriptorWrites.emplace_back() };

		//	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//	descriptorWrite.dstSet = mDescriptorSet;
		//	descriptorWrite.dstBinding = mBindings[uniformName].binding;
		//	descriptorWrite.descriptorCount = 1;
		//	descriptorWrite.descriptorType = Utils::GetVulkanDescriptorType(mBindings[uniformName].type);
		//	descriptorWrite.pImageInfo = &imageInfo;
		//}

		std::shared_ptr<VulkanTexture2D> vkTexture;
		if (mTextures.find("DiffuseMap") != mTextures.end())
		{
			vkTexture = mTextures["DiffuseMap"]->As<VulkanTexture2D>();
		}
		else
		{
			vkTexture = Renderer::GetDefaultTexture()->As<VulkanTexture2D>();
		}
		{
			VkDescriptorImageInfo imageInfo;
			imageInfo.sampler = vkTexture->GetSampler();
			imageInfo.imageView = vkTexture->GetImageView();
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			auto& descriptorWrite{ descriptorWrites.emplace_back() };

			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = mDescriptorSet;
			descriptorWrite.dstBinding = 0;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.descriptorType = Utils::GetVulkanDescriptorType(DescriptorType::COMBINED_IMAGE_SAMPLER);
			descriptorWrite.pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		if (mTextures.find("NormalMap") != mTextures.end())
		{
			vkTexture = mTextures["NormalMap"]->As<VulkanTexture2D>();
		}
		else
		{
			vkTexture = Renderer::GetDefaultNormalTexture()->As<VulkanTexture2D>();
		}
		{
			VkDescriptorImageInfo imageInfo;
			imageInfo.sampler = vkTexture->GetSampler();
			imageInfo.imageView = vkTexture->GetImageView();
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			auto& descriptorWrite{ descriptorWrites.emplace_back() };

			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = mDescriptorSet;
			descriptorWrite.dstBinding = 1;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.descriptorType = Utils::GetVulkanDescriptorType(DescriptorType::COMBINED_IMAGE_SAMPLER);
			descriptorWrite.pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		if (mTextures.find("RoughnessMap") != mTextures.end())
		{
			vkTexture = mTextures["RoughnessMap"]->As<VulkanTexture2D>();
		}
		else
		{
			vkTexture = Renderer::GetDefaultTexture()->As<VulkanTexture2D>();
		}
		{
			VkDescriptorImageInfo imageInfo;
			imageInfo.sampler = vkTexture->GetSampler();
			imageInfo.imageView = vkTexture->GetImageView();
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			auto& descriptorWrite{ descriptorWrites.emplace_back() };

			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = mDescriptorSet;
			descriptorWrite.dstBinding = 2;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.descriptorType = Utils::GetVulkanDescriptorType(DescriptorType::COMBINED_IMAGE_SAMPLER);
			descriptorWrite.pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}
}