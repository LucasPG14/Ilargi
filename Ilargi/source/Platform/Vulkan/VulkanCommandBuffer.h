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

		VkCommandBuffer GetCurrentCommand(uint32_t index) { return commandBuffers[index]; }

	private:
		std::vector<VkCommandBuffer> commandBuffers;
	};
}