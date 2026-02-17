#include "ilargipch.h"

#include "VulkanUniformBuffer.h"
#include "VulkanGraphicsContext.h"
#include "VulkanShader.h"
#include "Renderer/Renderer.h"

namespace Ilargi
{
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t aSize, uint32_t aFramesInFlight) : mSize(aSize)
	{
		const VkDevice& device{ VulkanGraphicsContext::GetLogicalDevice() };

		mUbos.resize(aFramesInFlight);
		mUniformBuffersMapped.resize(aFramesInFlight);

		VkBufferCreateInfo bufferInfo
		{
			.sType {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO},
			.pNext {nullptr},
			.flags {0U},
			.size {mSize},
			.usage {VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT},
			.sharingMode {VK_SHARING_MODE_EXCLUSIVE},
			.queueFamilyIndexCount {0U},
			.pQueueFamilyIndices {nullptr}
		};

		for (uint32_t i { 0 }; i < aFramesInFlight; ++i)
		{
			VulkanAllocator::AllocateBuffer(mUbos[i], bufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU);
			mUniformBuffersMapped[i] = VulkanAllocator::MapMemory(mUbos[i]);
		}

		auto vulkanShader{ Renderer::GetShader("PBR_Static")->As<VulkanShader>() };

		mDescriptorSets.resize(Renderer::GetConfig().maxFrames, VK_NULL_HANDLE);
		for (uint32_t setIndex { 0U }; setIndex < Renderer::GetConfig().maxFrames; ++setIndex)
		{
			vulkanShader->AllocateDescriptorSet(0, mDescriptorSets[setIndex]);
		}
	}
	
	VulkanUniformBuffer::~VulkanUniformBuffer()
	{

	}

	void VulkanUniformBuffer::Destroy()
	{
		vkDeviceWaitIdle(VulkanGraphicsContext::GetLogicalDevice());

		for (uint32_t i { 0U }; i < mUbos.size(); ++i)
		{
			VulkanAllocator::UnmapMemory(mUbos[i]);
			VulkanAllocator::DestroyBuffer(mUbos[i]);
		}
	}
	
	void VulkanUniformBuffer::SetData(void* aData, uint32_t aBinding)
	{
		auto device{ VulkanGraphicsContext::GetLogicalDevice() };
		uint32_t currentFrame{ Renderer::GetCurrentFrame() };

		memcpy(mUniformBuffersMapped[currentFrame], aData, mSize);

		VkDescriptorBufferInfo bufferInfo
		{
			.buffer {mUbos[currentFrame].buffer},
			.offset {0U},
			.range {mSize}
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