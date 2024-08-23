#include "ilargipch.h"

#include "VulkanUniformBuffer.h"
#include "VulkanContext.h"
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
		uint32_t currentFrame = Renderer::GetCurrentFrame();

		memcpy(mUniformBuffersMapped[currentFrame], data, mSize);
	}
}