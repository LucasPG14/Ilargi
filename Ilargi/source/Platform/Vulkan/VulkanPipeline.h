#pragma once

#include "Renderer/Pipeline.h"
#include "Renderer/Framebuffer.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineProperties& aProperties);
		virtual ~VulkanPipeline();

		void Init(VkRenderPass aRenderPass, const std::vector<ImageFormat>& aFormats);
		void Destroy();

		void PushConstants(const std::shared_ptr<CommandBuffer>& aCommandBuffer, uint32_t aOffset, uint32_t aSize, const void* aData) const override;

		void Bind(const std::shared_ptr<CommandBuffer>& aCommandBuffer) const override;
		void BindDescriptorSet(const std::shared_ptr<CommandBuffer>& aCommandBuffer, std::shared_ptr<Material> aMaterial, uint32_t aSetIndex) const override;
		void BindDescriptorSet(const std::shared_ptr<CommandBuffer>& aCommandBuffer, std::shared_ptr<UniformBuffer> aUniformBuffer, uint32_t aSetIndex) const override;

		const PipelineProperties& GetProperties() const override { return mProperties; }
	private:
		PipelineProperties mProperties;

		VkPipeline mPipeline;
		VkPipelineLayout mPipelineLayout;
		VkDescriptorSetLayout mDescriptorSetLayout;
	};
}