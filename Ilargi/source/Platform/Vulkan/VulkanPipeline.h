#pragma once

#include "Renderer/Pipeline.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineProperties& props);
		virtual ~VulkanPipeline();

		void Init(VkRenderPass renderPass);
		void Destroy();

		void PushConstants(std::shared_ptr<CommandBuffer> commandBuffer, uint32_t offset, uint32_t size, void* data) const override;

		void Bind(std::shared_ptr<CommandBuffer> commandBuffer) const override;
		void BindDescriptorSet(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<Material> material) const override;

		const PipelineProperties& GetProperties() const override { return properties; }
	private:
		PipelineProperties properties;

		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
		VkDescriptorSetLayout descriptorSetLayout;
	};
}