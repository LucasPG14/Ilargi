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

		void Destroy();

		void Bind(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer) override;
		void Unbind(std::shared_ptr<CommandBuffer> commandBuffer) override;

		const PipelineProperties& GetProperties() const override { return properties; }
	private:
		PipelineProperties properties;

		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
	};
}