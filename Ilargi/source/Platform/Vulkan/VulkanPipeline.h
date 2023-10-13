#pragma once

#include "Renderer/Pipeline.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class CommandBuffer;

	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineProperties& props);
		virtual ~VulkanPipeline();

		void Init(VkRenderPass renderPass);
		void Destroy();

		void PushConstants(std::shared_ptr<CommandBuffer> commandBuffer, uint32_t offset, uint32_t size, void* data) override;

		void Bind(std::shared_ptr<CommandBuffer> commandBuffer) override;

		const PipelineProperties& GetProperties() const override { return properties; }
	private:
		PipelineProperties properties;

		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
		VkDescriptorSetLayout descriptorSetLayout;
	};
}