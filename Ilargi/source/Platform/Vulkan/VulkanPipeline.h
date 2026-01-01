#pragma once

#include "Renderer/Pipeline.h"
#include "Renderer/Framebuffer.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanPipeline : public Pipeline
	{
	public:
		/*
		* @brief Constructor.
		* @param aProperties The properties of the pipeline.
		*/
		VulkanPipeline(const PipelineProperties& aProperties);
		
		/*
		* @brief Destructor.
		*/
		virtual ~VulkanPipeline();

		/*
		* @brief Initializes the pipeline.
		* @param aRenderPass The Vulkan render pass for the pipeline.
		* @param aFormats The image formats of the framebuffer.
		*/
		void Init(VkRenderPass aRenderPass, const std::vector<ImageFormat>& aFormats);
		
		/*
		* @copydoc Pipeline::Destroy().
		*/
		void Destroy() override;

		/*
		* @copydoc Pipeline::PushConstants().
		*/
		void PushConstants(const std::shared_ptr<CommandBuffer>& aCommandBuffer, uint32_t aOffset, uint32_t aSize, const void* aData) const override;

		/*
		* @copydoc Pipeline::Bind().
		*/
		void Bind(const std::shared_ptr<CommandBuffer>& aCommandBuffer) const override;
		
		/*
		* @copydoc Pipeline::BindMaterial().
		*/
		void BindMaterial(const std::shared_ptr<CommandBuffer>& aCommandBuffer, const std::shared_ptr<Material>& aMaterial, uint32_t aSetIndex) const override;
		
		/*
		* @copydoc Pipeline::BindUniformBuffer().
		*/
		void BindUniformBuffer(const std::shared_ptr<CommandBuffer>& aCommandBuffer, const std::shared_ptr<UniformBuffer>& aUniformBuffer, uint32_t aSetIndex) const override;

		/*
		* @copydoc Pipeline::GetProperties().
		*/
		[[nodiscard]] const PipelineProperties& GetProperties() const override { return mProperties; }
	
	private:
		PipelineProperties mProperties; // The pipeline properties.

		VkPipeline mPipeline; // The vulkan pipeline.
		VkPipelineLayout mPipelineLayout; // The vulkan pipeline layout.
		VkDescriptorSetLayout mDescriptorSetLayout; // The layout of the vulkan descriptor sets.
	};
}