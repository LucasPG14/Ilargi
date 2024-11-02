#include "ilargipch.h"

#include "VulkanUniformBuffer.h"
#include "VulkanContext.h"
#include "VulkanShader.h"
#include "Renderer/Renderer.h"

namespace Ilargi
{
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t s, uint32_t framesInFlight) : mSize(s)
	{
		auto device = VulkanContext::GetLogicalDevice();

		mUbos.resize(framesInFlight);
		mUniformBuffersMapped.resize(framesInFlight);

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = mSize;
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		for (size_t i = 0; i < framesInFlight; i++)
		{
			VulkanAllocator::AllocateBuffer(mUbos[i], bufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU);
			mUniformBuffersMapped[i] = VulkanAllocator::MapMemory(mUbos[i]);
		}

		auto vulkanShader = std::static_pointer_cast<VulkanShader>(Renderer::GetShaderLibrary()->Get("PBR_Static"));

		mDescriptorSets.resize(Renderer::GetConfig().maxFrames, VK_NULL_HANDLE);
		for (int i = 0; i < Renderer::GetConfig().maxFrames; ++i)
		{
//			vulkanShader->AllocateDescriptorSet(1, mDescriptorSets[i]);
		}
	}
	
	VulkanUniformBuffer::~VulkanUniformBuffer()
	{

	}

	void VulkanUniformBuffer::Destroy()
	{
		vkDeviceWaitIdle(VulkanContext::GetLogicalDevice());

		for (int i = 0; i < mUbos.size(); ++i)
		{
			VulkanAllocator::UnmapMemory(mUbos[i]);
			VulkanAllocator::DestroyBuffer(mUbos[i]);
		}
	}
	
	void VulkanUniformBuffer::SetData(void* data)
	{
		auto device = VulkanContext::GetLogicalDevice();
		uint32_t currentFrame = Renderer::GetCurrentFrame();

		memcpy(mUniformBuffersMapped[currentFrame], data, mSize);

		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = mUbos[currentFrame].buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = mSize;

		std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

		for (int i = 0; i < 3; ++i)
		{
			descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[i].dstSet = mDescriptorSets[currentFrame];
			descriptorWrites[i].dstBinding = 0;
			descriptorWrites[i].dstArrayElement = 0;
			descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[i].descriptorCount = 1;
			descriptorWrites[i].pBufferInfo = &bufferInfo;
		}

		//vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
	
	const void* VulkanUniformBuffer::GetDescriptorSet() const
	{
		return mDescriptorSets[Renderer::GetCurrentFrame()];
	}
}