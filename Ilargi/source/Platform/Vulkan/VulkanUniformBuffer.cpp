#include "ilargipch.h"

#include "VulkanUniformBuffer.h"
#include "VulkanContext.h"
#include "VulkanShader.h"
#include "Renderer/Renderer.h"

namespace Ilargi
{
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t s, uint32_t framesInFlight) : mSize(s)
	{
		auto device{ VulkanContext::GetLogicalDevice() };

		mUbos.resize(framesInFlight);
		mUniformBuffersMapped.resize(framesInFlight);

		VkBufferCreateInfo bufferInfo
		{
			VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,	// sType
			nullptr,								// pNext
			0,										// flags
			mSize,									// size
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,		// usage
			VK_SHARING_MODE_EXCLUSIVE,				// sharingMode
			0,										// queueFamilyIndexCount
			nullptr									// pQueueFamilyIndices
		};

		for (uint32_t i { 0 }; i < framesInFlight; ++i)
		{
			VulkanAllocator::AllocateBuffer(mUbos[i], bufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU);
			mUniformBuffersMapped[i] = VulkanAllocator::MapMemory(mUbos[i]);
		}

		auto vulkanShader{ Renderer::GetShader("PBR_Static")->As<VulkanShader>() };

		mDescriptorSets.resize(Renderer::GetConfig().maxFrames, VK_NULL_HANDLE);
		for (uint32_t setIndex { 0U }; setIndex < Renderer::GetConfig().maxFrames; ++setIndex)
		{
			vulkanShader->AllocateDescriptorSet(1, mDescriptorSets[setIndex]);
		}
	}
	
	VulkanUniformBuffer::~VulkanUniformBuffer()
	{

	}

	void VulkanUniformBuffer::Destroy()
	{
		vkDeviceWaitIdle(VulkanContext::GetLogicalDevice());

		for (uint32_t i { 0U }; i < mUbos.size(); ++i)
		{
			VulkanAllocator::UnmapMemory(mUbos[i]);
			VulkanAllocator::DestroyBuffer(mUbos[i]);
		}
	}
	
	void VulkanUniformBuffer::SetData(void* aData, uint32_t aBinding)
	{
		auto device{ VulkanContext::GetLogicalDevice() };
		uint32_t currentFrame{ Renderer::GetCurrentFrame() };

		memcpy(mUniformBuffersMapped[currentFrame], aData, mSize);

		VkDescriptorBufferInfo bufferInfo
		{
			mUbos[currentFrame].buffer,		// buffer
			0,								// offset
			mSize							// range
		};

		std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

		for (uint32_t i { 0U }; i < 3U; ++i)
		{
			descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[i].dstSet = mDescriptorSets[currentFrame];
			descriptorWrites[i].dstBinding = aBinding;
			descriptorWrites[i].dstArrayElement = 0;
			descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[i].descriptorCount = 1;
			descriptorWrites[i].pBufferInfo = &bufferInfo;
		}

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
	
	const VkDescriptorSet VulkanUniformBuffer::GetDescriptorSet() const
	{
		return mDescriptorSets[Renderer::GetCurrentFrame()];
	}
}