#pragma once

#include "Renderer/ICommandBuffer.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanCommandBuffer : public ICommandBuffer
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
		* @copydoc CommandBuffer::BeginRenderPass()
		*/
		void BeginRenderPass(const IRenderPass& aRenderPass, const std::shared_ptr<IFramebuffer>& aFramebuffer) const override;
		
		/*
		* @copydoc CommandBuffer::EndRenderPass()
		*/
		void EndRenderPass() const override;

		/*
		* @copydoc CommandBuffer::BindPipeline()
		*/
		void BindPipeline(const std::shared_ptr<IGraphicsPipeline>& aPipeline) const override;

		/*
		* @copydoc CommandBuffer::PushConstants().
		*/
		void PushConstants(std::string_view aShaderName, uint32_t aOffset, uint32_t aSize, const void* aData) const override;

		/*
		* @copydoc CommandBuffer::BindMaterial().
		*/
		void BindMaterial(std::string_view aShaderName, const std::shared_ptr<Material>& aMaterial, uint32_t aSetIndex) const override;

		/*
		* @copydoc CommandBuffer::BindUniformBuffer().
		*/
		void BindUniformBuffer(std::string_view aShaderName, const std::shared_ptr<IUniformBuffer>& aUniformBuffer, uint32_t aSetIndex) const override;

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