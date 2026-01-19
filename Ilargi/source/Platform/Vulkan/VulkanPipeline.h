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
		* @param aFormats The image formats of the framebuffer.
		*/
		void Init(const std::vector<ImageFormat>& aFormats);
		
		/*
		* @copydoc Pipeline::Destroy().
		*/
		void Destroy() override;

		/*
		* @copydoc Pipeline::PushConstants().
		*/
		void PushConstants(const std::shared_ptr<CommandBuffer>& aCommandBuffer, ShaderStage aShaderStage, uint32_t aOffset, uint32_t aSize, const void* aData) const override;

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
		/*
		* @brief Returns a vulkan depth stencil state based on the given depth state.
		* @param aDepthState The depth state information to build the vulkan depth stencil state.
		* @return The vulkan depth stencil state.
		*/
		VkPipelineDepthStencilStateCreateInfo GetDepthStencilState(const DepthState& aDepthState);

		/*
		* @brief Returns a vulkan stencil state based on the given stencil face state.
		* @param aStencilFaceState The stencil face state information to build the vulkan stencil state.
		* @return The vulkan stencil state.
		*/
		VkStencilOpState GetStencilOpState(const StencilFaceState& aStencilFaceState);
		
		/*
		* @brief Returns a vulkan color blend attachment based on the given blend state.
		* @param aBlendState The blend state information to build the vulkan blend state.
		* @return The vulkan color blend attachment.
		*/
		VkPipelineColorBlendAttachmentState GetBlendState(const BlendState& aBlendState);

		/*
		* @brief Returns a vulkan rasterization state based on the given raster state.
		* @param aRasterState The raster state information to build the vulkan rasterization state.
		* @return The vulkan rasterization state.
		*/
		VkPipelineRasterizationStateCreateInfo GetRasterizationState(const RasterState& aRasterState);

	private:
		PipelineProperties mProperties; // The pipeline properties.

		VkPipeline mPipeline; // The vulkan pipeline.
	};
}