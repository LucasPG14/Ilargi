#pragma once

#include "Renderer/CommandBuffer.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanCommandBuffer : public CommandBuffer
	{
	public:
		/*
		* @brief Constructor.
		* @param aFramesInFlight Number of command buffers needed.
		*/
		VulkanCommandBuffer(uint32_t aFramesInFlight);

		/*
		* @brief Destructor.
		*/
		virtual ~VulkanCommandBuffer();

		/*
		* @copydoc CommandBuffer::Destroy()
		*/
		void Destroy() const override;

		/*
		* @copydoc CommandBuffer::BeginCommand()
		*/
		void BeginCommand() const override;

		/*
		* @copydoc CommandBuffer::EndCommand()
		*/
		void EndCommand() const override;

		/*
		* @copydoc CommandBuffer::Submit()
		*/
		void Submit() const override;

		/*
		* @brief Gets the current command.
		* @param aIndex The frame of the current command buffer.
		* @return An instance of the VkCommandBuffer.
		*/
		[[nodiscard]] const VkCommandBuffer GetCurrentCommand(uint32_t aIndex) const { return mCommandBuffers[aIndex]; }

	private:
		std::vector<VkCommandBuffer> mCommandBuffers; // Container of the command buffers.
		std::vector<VkQueryPool> mQueryPools;
		std::vector<uint64_t> mTimeQuery;
		VkFence mFence;
		uint32_t mQueryPoolCount;
	};
}