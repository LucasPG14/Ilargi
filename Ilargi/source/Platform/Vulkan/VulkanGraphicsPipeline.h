#pragma once

#include "Renderer/IGraphicsPipeline.h"
#include "Renderer/IFramebuffer.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanGraphicsPipeline : public IGraphicsPipeline
	{
	public:
		/*
		* @brief Constructor.
		* @param aProperties The properties of the pipeline.
		*/
		VulkanGraphicsPipeline(const GraphicsPipelineProperties& aProperties);
		
		/*
		* @brief Destructor.
		*/
		virtual ~VulkanGraphicsPipeline();

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
		* @copydoc Pipeline::GetProperties().
		*/
		[[nodiscard]] const GraphicsPipelineProperties& GetProperties() const override { return mProperties; }
		
		/*
		* @brief Returns the vulkan pipeline.
		* @return The pipeline handle.
		*/
		[[nodiscard]] const VkPipeline& GetPipeline() const { return mPipeline; }
	
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
		GraphicsPipelineProperties mProperties; // The pipeline properties.

		VkPipeline mPipeline; // The vulkan pipeline.
	};
}