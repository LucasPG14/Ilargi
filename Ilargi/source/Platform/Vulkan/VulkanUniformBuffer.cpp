#include "ilargipch.h"

#include "VulkanUniformBuffer.h"
#include "VulkanContext.h"
#include "Renderer/Renderer.h"

namespace Ilargi
{
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t s, uint32_t framesInFlight) : size(s)
	{
		auto device = VulkanContext::GetLogicalDevice();

		ubos.resize(framesInFlight);
		uniformBuffersMapped.resize(framesInFlight);

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		for (size_t i = 0; i < framesInFlight; i++)
		{
			VulkanAllocator::AllocateBuffer(ubos[i], bufferInfo, VMA_MEMORY_USAGE_CPU_TO_GPU);
			uniformBuffersMapped[i] = VulkanAllocator::MapMemory(ubos[i]);
		}
	}
	
	VulkanUniformBuffer::~VulkanUniformBuffer()
	{

	}

	void VulkanUniformBuffer::Destroy()
	{
		vkDeviceWaitIdle(VulkanContext::GetLogicalDevice());

		for (int i = 0; i < ubos.size(); ++i)
		{
			VulkanAllocator::UnmapMemory(ubos[i]);
			VulkanAllocator::DestroyBuffer(ubos[i]);
		}
	}
	
	void VulkanUniformBuffer::SetData(void* data)
	{
		uint32_t currentFrame = Renderer::GetCurrentFrame();

		memcpy(uniformBuffersMapped[currentFrame], data, size);
	}
}