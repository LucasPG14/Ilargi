#pragma once

#include "Renderer/CommandBuffer.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer(uint32_t framesInFlight);
		virtual ~VulkanCommandBuffer();

		void BeginCommand() const override;
		void EndCommand() const override;

		void Submit() const override;

		VkCommandBuffer GetCurrentCommand(uint32_t index) { return commandBuffers[index]; }

	private:
		std::vector<VkCommandBuffer> commandBuffers;
		VkFence fence;

		std::vector<VkQueryPool> queryPools;
		uint32_t queryPoolCount;
		std::vector<uint64_t> timeQuery;
	};
}