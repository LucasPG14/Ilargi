#pragma once

#include "Renderer/CommandBuffer.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer(uint32_t aFramesInFlight);
		virtual ~VulkanCommandBuffer();

		void Destroy() const override;

		void BeginCommand() const override;
		void EndCommand() const override;

		void Submit() const override;

		[[nodiscard]] const VkCommandBuffer GetCurrentCommand(uint32_t aIndex) const { return mCommandBuffers[aIndex]; }

	private:
		std::vector<VkCommandBuffer> mCommandBuffers;
		VkFence mFence;

		std::vector<VkQueryPool> mQueryPools;
		uint32_t mQueryPoolCount;
		std::vector<uint64_t> mTimeQuery;
	};
}