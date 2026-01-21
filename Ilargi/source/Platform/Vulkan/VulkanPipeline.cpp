#include "ilargipch.h"

#include "VulkanPipeline.h"
#include "Renderer/Renderer.h"
#include "Resources/Material.h"
#include "VulkanContext.h"
#include "VulkanRenderPass.h"
#include "VulkanCommandBuffer.h"
#include "VulkanMaterial.h"
#include "VulkanUniformBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanShader.h"

#include "Utils/FileSystem.h"

namespace Ilargi
{
	namespace Utils
	{
		VkFormat GetVkFormatFromShaderDataType(ShaderDataType type)
		{
			switch (type)
			{
			case ShaderDataType::FLOAT_16:		return VK_FORMAT_R16_SFLOAT;
			case ShaderDataType::FLOAT2_16:		return VK_FORMAT_R16G16_SFLOAT;
			case ShaderDataType::FLOAT3_16:		return VK_FORMAT_R16G16B16_SFLOAT;
			case ShaderDataType::FLOAT4_16:		return VK_FORMAT_R16G16B16A16_SFLOAT;

			case ShaderDataType::FLOAT_32:		return VK_FORMAT_R32_SFLOAT;
			case ShaderDataType::FLOAT2_32:		return VK_FORMAT_R32G32_SFLOAT;
			case ShaderDataType::FLOAT3_32:		return VK_FORMAT_R32G32B32_SFLOAT;
			case ShaderDataType::FLOAT4_32:		return VK_FORMAT_R32G32B32A32_SFLOAT;

			case ShaderDataType::INT_16:		return VK_FORMAT_R16_SINT;
			case ShaderDataType::INT2_16:		return VK_FORMAT_R16G16_SINT;
			case ShaderDataType::INT3_16:		return VK_FORMAT_R16G16B16_SINT;
			case ShaderDataType::INT4_16:		return VK_FORMAT_R16G16B16A16_SINT;

			case ShaderDataType::INT_32:		return VK_FORMAT_R32_SINT;
			case ShaderDataType::INT2_32:		return VK_FORMAT_R32G32_SINT;
			case ShaderDataType::INT3_32:		return VK_FORMAT_R32G32B32_SINT;
			case ShaderDataType::INT4_32:		return VK_FORMAT_R32G32B32A32_SINT;
			}

			ILG_ASSERT(nullptr, "Vk format not found for Shader Data Type")
			return VkFormat();
		}

		const std::filesystem::path GetPipelineCacheDirectory()
		{
			return { "Cache/vulkan/pipelines/" };
		}

		void CreatePipelineCacheDirectory()
		{
			const std::filesystem::path& cacheDirectory{ GetPipelineCacheDirectory() };
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		VkStencilOp GetVulkanStencilOp(StencilOp aStencilOp)
		{
			switch(aStencilOp)
			{
			case StencilOp::KEEP:				return VK_STENCIL_OP_KEEP;
			case StencilOp::ZERO:				return VK_STENCIL_OP_ZERO;
			case StencilOp::REPLACE:			return VK_STENCIL_OP_REPLACE;
			case StencilOp::INCREMENT_WRAP:		return VK_STENCIL_OP_INCREMENT_AND_WRAP;
			case StencilOp::INCREMENT_CLAMP:	return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
			case StencilOp::DECREMENT_WRAP:		return VK_STENCIL_OP_DECREMENT_AND_WRAP;
			case StencilOp::DECREMENT_CLAMP:	return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
			case StencilOp::INVERT:				return VK_STENCIL_OP_INVERT;
			}

			return VK_STENCIL_OP_KEEP;
		}

		VkCompareOp GetVulkanCompareOp(CompareOp aCompareOp)
		{
			switch (aCompareOp)
			{
			case CompareOp::NEVER:			return VK_COMPARE_OP_NEVER;
			case CompareOp::LESS:			return VK_COMPARE_OP_LESS;
			case CompareOp::EQUAL:			return VK_COMPARE_OP_EQUAL;
			case CompareOp::LESS_EQUAL:		return VK_COMPARE_OP_LESS_OR_EQUAL;
			case CompareOp::GREATER:		return VK_COMPARE_OP_GREATER;
			case CompareOp::NOT_EQUAL:		return VK_COMPARE_OP_NOT_EQUAL;
			case CompareOp::GREATER_EQUAL:	return VK_COMPARE_OP_GREATER_OR_EQUAL;
			case CompareOp::ALWAYS:			return VK_COMPARE_OP_ALWAYS;
			}

			return VK_COMPARE_OP_ALWAYS;
		}

		VkPolygonMode GetVulkanFillMode(FillMode aFillMode)
		{
			switch (aFillMode)
			{
			case FillMode::FILL:	return VK_POLYGON_MODE_FILL;
			case FillMode::LINE:	return VK_POLYGON_MODE_LINE;
			case FillMode::POINT:	return VK_POLYGON_MODE_POINT;
			}

			return VK_POLYGON_MODE_FILL;
		}

		VkCullModeFlags GetVulkanCullMode(CullMode aCullMode)
		{
			switch (aCullMode)
			{
			case CullMode::NONE:	return VK_CULL_MODE_NONE;
			case CullMode::FRONT:	return VK_CULL_MODE_FRONT_BIT;
			case CullMode::BACK:	return VK_CULL_MODE_BACK_BIT;
			}

			return VK_CULL_MODE_BACK_BIT;
		}

		VkFrontFace GetVulkanFrontFace(FrontFace aFrontFace)
		{
			switch (aFrontFace)
			{
			case FrontFace::COUNTER_CLOCKWISE:		return VK_FRONT_FACE_COUNTER_CLOCKWISE;
			case FrontFace::CLOCKWISE:				return VK_FRONT_FACE_CLOCKWISE;
			}

			return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		}

		VkBlendFactor GetVulkanBlendFactor(BlendFactor aBlendFactor)
		{
			switch (aBlendFactor)
			{
			case BlendFactor::ZERO:							return VK_BLEND_FACTOR_ZERO;
			case BlendFactor::ONE:							return VK_BLEND_FACTOR_ONE;
			case BlendFactor::SRC_COLOR:					return VK_BLEND_FACTOR_SRC_COLOR;
			case BlendFactor::ONE_MINUS_SRC_COLOR:			return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			case BlendFactor::DST_COLOR:					return VK_BLEND_FACTOR_DST_COLOR;
			case BlendFactor::ONE_MINUS_DST_COLOR:			return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
			case BlendFactor::SRC_ALPHA:					return VK_BLEND_FACTOR_SRC_ALPHA;
			case BlendFactor::ONE_MINUS_SRC_ALPHA:			return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			case BlendFactor::DST_ALPHA:					return VK_BLEND_FACTOR_DST_ALPHA;
			case BlendFactor::ONE_MINUS_DST_ALPHA:			return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
			case BlendFactor::CONSTANT_COLOR:				return VK_BLEND_FACTOR_CONSTANT_COLOR;
			case BlendFactor::ONE_MINUS_CONSTANT_COLOR:		return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
			}

			return VK_BLEND_FACTOR_ZERO;
		}

		VkBlendOp GetVulkanBlendOp(BlendOp aBlendOp)
		{
			switch (aBlendOp)
			{
			case BlendOp::ADD:					return VK_BLEND_OP_ADD;
			case BlendOp::SUBSTRACT:			return VK_BLEND_OP_SUBTRACT;
			case BlendOp::REVERSE_SUBSTRACT:	return VK_BLEND_OP_REVERSE_SUBTRACT;
			case BlendOp::MIN:					return VK_BLEND_OP_MIN;
			case BlendOp::MAX:					return VK_BLEND_OP_MAX;
			}

			return VK_BLEND_OP_ADD;
		}

		VkColorComponentFlags GetVulkanColorMask(ColorMask aColorMask)
		{
			switch (aColorMask)
			{
			case ColorMask::R:		return VK_COLOR_COMPONENT_R_BIT;
			case ColorMask::RG:		return VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT;
			case ColorMask::RGB:	return VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
			case ColorMask::RGBA:	return VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			}

			return VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		}
	}

	VulkanPipeline::VulkanPipeline(const PipelineProperties& aProperties) 
		: mProperties(aProperties), mPipeline(VK_NULL_HANDLE)
	{
		Utils::CreatePipelineCacheDirectory();
		Init(aProperties.ColorFormats);
	}
	
	VulkanPipeline::~VulkanPipeline()
	{
		const auto& device{ VulkanContext::GetLogicalDevice() };

		vkDestroyPipeline(device, mPipeline, nullptr);
	}

	void VulkanPipeline::Init(const std::vector<ImageFormat>& aFormats)
	{
		auto device{ VulkanContext::GetLogicalDevice() };

		// Pipeline cache
		//VkPipelineCache pipelineCache{ VK_NULL_HANDLE };

		//VkPipelineCacheCreateInfo pipelineCacheInfo {};
		//pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

		//std::filesystem::path pipelineCacheFilepath{ (std::filesystem::path("cache/vulkan/pipelines/" + mProperties.name + ".pipe")) };
		//BinaryReader reader(pipelineCacheFilepath);

		//if (std::filesystem::exists(pipelineCacheFilepath))
		//{
		//	pipelineCacheInfo.initialDataSize = reader.GetSize();
		//	
		//	char* cacheData = new char[pipelineCacheInfo.initialDataSize];
		//	reader.Read(cacheData, pipelineCacheInfo.initialDataSize);
		//	
		//	pipelineCacheInfo.pInitialData = cacheData;
		//	delete[] cacheData;
		//}
		//VK_CHECK_RESULT(vkCreatePipelineCache(device, &pipelineCacheInfo, nullptr, &pipelineCache));

		VkGraphicsPipelineCreateInfo pipelineInfo {};

		const auto& shader{ Renderer::GetShader(mProperties.ShaderName)->As<VulkanShader>() };

		const auto& shaders{ shader->GetShaders() };
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		for (auto&& [type, shaderModule] : shaders)
		{
			VkPipelineShaderStageCreateInfo shaderStageInfo
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,	// sType
				nullptr,												// pNext
				0,														// flags
				type,													// stage
				shaderModule,											// module
				"main",													// pName
				nullptr													// pSpecializationInfo
			};

			shaderStages.push_back(shaderStageInfo);
		}

		// TODO: Gives an error when layout is empty
		VkVertexInputBindingDescription bindingDescription
		{
			0,										// binding
			mProperties.VertexLayout.GetStride(),	// stride
			VK_VERTEX_INPUT_RATE_VERTEX				// inputRate
		};

		const auto& elements{ mProperties.VertexLayout.GetElements() };
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(elements.size());

		int i{ 0 };
		for (auto& attributeDescription : attributeDescriptions)
		{
			attributeDescription.binding = 0;
			attributeDescription.location = i;
			attributeDescription.format = Utils::GetVkFormatFromShaderDataType(elements[i].Type);
			attributeDescription.offset = elements[i].Offset;

			i++;
		}

		uint32_t attributeSize{ static_cast<uint32_t>(attributeDescriptions.size()) };
		VkPipelineVertexInputStateCreateInfo vertexInputInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,					// sType
			nullptr,																	// pNext
			0,																			// flags
			attributeSize == 0 ? 0U : 1U,												// vertexBindingDescriptionCount
			attributeSize == 0 ? VK_NULL_HANDLE : &bindingDescription,					// pVertexBindingDescriptions
			attributeSize == 0 ? 0U : attributeSize,									// vertexAttributeDescriptionCount
			attributeSize == 0 ? VK_NULL_HANDLE : attributeDescriptions.data()			// pVertexAttributeDescriptions
		};

		VkPipelineInputAssemblyStateCreateInfo inputAssembly
		{
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,	// sType
			nullptr,														// pNext
			0,																// flags
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,							// topology
			VK_FALSE														// primitiveRestartEnable
		};

		uint32_t width{ 1080U };
		uint32_t height{ 720U };

		VkViewport viewport
		{
			0.0f,			// x
			0.0f,			// y
			(float)width,	// width
			(float)height,	// height
			0.0f,			// minDepth
			1.0f			// maxDepth
		};

		VkRect2D scissor
		{
			{ 0, 0 },			// offset
			{ width, height }	// extent
		};

		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState
		{
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,	// sType
			nullptr,												// pNext
			0,														// flags
			static_cast<uint32_t>(dynamicStates.size()),			// dynamicStateCount
			dynamicStates.data()									// pDynamicStates
		};

		VkPipelineViewportStateCreateInfo viewportState
		{
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,	// sType
			nullptr,												// pNext
			0,														// flags
			1,														// viewportCount
			&viewport,												// pViewports
			1,														// scissorCount
			&scissor												// pScissors
		};

		if (mProperties.DepthState.Enabled)
		{
			VkPipelineDepthStencilStateCreateInfo depthStencil{ GetDepthStencilState(mProperties.DepthState) };

			pipelineInfo.pDepthStencilState = &depthStencil;
		}

		VkPipelineRasterizationStateCreateInfo rasterizer{ GetRasterizationState(mProperties.RasterState) };

		VkPipelineMultisampleStateCreateInfo multisampling
		{
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,	// sType
			nullptr,													// pNext
			0,															// flags
			VK_SAMPLE_COUNT_1_BIT,										// rasterizationSamples
			VK_FALSE,													// sampleShadingEnable
			1.0f,														// minSampleShading
			nullptr,													// pSampleMask
			VK_FALSE,													// alphaToCoverageEnable
			VK_FALSE													// alphaToOneEnable
		};

		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
		colorBlendAttachments.reserve(aFormats.size());
		
		for (uint32_t i { 0 }; i < aFormats.size(); ++i)
		{
			if (Utils::IsDepth(aFormats[i]))
				continue;

			colorBlendAttachments.push_back(GetBlendState(mProperties.BlendState));
		}

		VkPipelineColorBlendStateCreateInfo colorBlending
		{
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,	// sType
			nullptr,													// pNext
			0,															// flags
			VK_FALSE,													// logicOpEnable
			VK_LOGIC_OP_COPY,											// logicOp
			static_cast<uint32_t>(colorBlendAttachments.size()),		// attachmentCount
			colorBlendAttachments.data(),								// pAttachments
			{ 0.0f, 0.0f, 0.0f, 0.0f }									// blendConstants
		};

		if (mProperties.DepthState.Enabled)
		{
			VkPipelineDepthStencilStateCreateInfo depthStencil
			{
				VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, // sType
				nullptr,													// pNext
				0,															// flags
				mProperties.DepthState.Test,								// depthTestEnable
				mProperties.DepthState.Write,								// depthWriteEnable
				VK_COMPARE_OP_LESS,											// depthCompareOp
				VK_FALSE,													// depthBoundsTestEnable
				mProperties.DepthState.StencilState.Enabled,				// stencilTestEnable
				{},															// front
				{},															// back
				0.0f,														// minDepthBounds
				1.0f														// maxDepthBounds
			};

			if (mProperties.DepthState.StencilState.Enabled)
			{
				depthStencil.front = GetStencilOpState(mProperties.DepthState.StencilState.Front);
				depthStencil.back = GetStencilOpState(mProperties.DepthState.StencilState.Back);
			}
			pipelineInfo.pDepthStencilState = &depthStencil;
		}
		
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineInfo.pStages = shaderStages.data();

		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;

		pipelineInfo.layout = shader->GetPipelineLayout();

		pipelineInfo.renderPass = Renderer::GetRenderPass({aFormats, true})->As<VulkanRenderPass>()->GetRenderPass();
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mPipeline));

		//if (!std::filesystem::exists(pipelineCacheFilepath))
		//{
		//	size_t pipelineSize;
		//	VK_CHECK_RESULT(vkGetPipelineCacheData(device, pipelineCache, &pipelineSize, nullptr));

		//	char* data = new char[pipelineSize];
		//	VK_CHECK_RESULT(vkGetPipelineCacheData(device, pipelineCache, &pipelineSize, data));

		//	BinaryWriter writer(pipelineCacheFilepath);
		//	writer.Write(data, pipelineSize);

		//	delete[] data;
		//}

		//vkDestroyPipelineCache(device, pipelineCache, nullptr);
	}

	void VulkanPipeline::Destroy()
	{
		auto device{ VulkanContext::GetLogicalDevice() };

		vkDestroyPipeline(device, mPipeline, nullptr);
	}

	void VulkanPipeline::PushConstants(const std::shared_ptr<CommandBuffer>& aCommandBuffer, ShaderStage aShaderStage, uint32_t aOffset, uint32_t aSize, const void* aData) const
	{
		Renderer::Submit([this, aCommandBuffer, aShaderStage, aOffset, aSize, aData]()
			{
				const uint32_t currentFrame{ Renderer::GetCurrentFrame() };

				const VkCommandBuffer cmdBuffer{ aCommandBuffer->As<VulkanCommandBuffer>()->GetCurrentCommand(currentFrame) };
				vkCmdPushConstants(cmdBuffer, Renderer::GetShader(mProperties.ShaderName)->As<VulkanShader>()->GetPipelineLayout(), VK_SHADER_STAGE_ALL_GRAPHICS, aOffset, aSize, aData);
			});
	}

	void VulkanPipeline::Bind(const std::shared_ptr<CommandBuffer>& aCommandBuffer) const
	{
		Renderer::Submit([this, aCommandBuffer]()
			{
				const uint32_t currentFrame{ Renderer::GetCurrentFrame() };

				const VkCommandBuffer cmdBuffer{ aCommandBuffer->As<VulkanCommandBuffer>()->GetCurrentCommand(currentFrame) };
				vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
			});
	}
	
	void VulkanPipeline::BindMaterial(const std::shared_ptr<CommandBuffer>& aCommandBuffer, const std::shared_ptr<Material>& aMaterial, uint32_t aSetIndex) const
	{
		Renderer::Submit([this, aCommandBuffer, aMaterial, aSetIndex]()
			{
				uint32_t currentFrame { Renderer::GetCurrentFrame() };

				const VkCommandBuffer cmdBuffer { aCommandBuffer->As<VulkanCommandBuffer>()->GetCurrentCommand(currentFrame) };
				std::vector<VkDescriptorSet> descriptorSets { aMaterial->As<VulkanMaterial>()->GetDescriptorSet()};
				vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::GetShader(mProperties.ShaderName)->As<VulkanShader>()->GetPipelineLayout(), aSetIndex, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
			});
	}

	void VulkanPipeline::BindUniformBuffer(const std::shared_ptr<CommandBuffer>& aCommandBuffer, const std::shared_ptr<UniformBuffer>& aUniformBuffer, uint32_t aSetIndex) const
	{
		Renderer::Submit([this, aCommandBuffer, aUniformBuffer, aSetIndex]()
			{
				uint32_t currentFrame { Renderer::GetCurrentFrame() };

				const VkCommandBuffer cmdBuffer { aCommandBuffer->As<VulkanCommandBuffer>()->GetCurrentCommand(currentFrame) };
				std::vector<VkDescriptorSet> descriptorSets { aUniformBuffer->As<VulkanUniformBuffer>()->GetDescriptorSet() };
				vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::GetShader(mProperties.ShaderName)->As<VulkanShader>()->GetPipelineLayout(), aSetIndex, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
			});
	}
	
	VkPipelineDepthStencilStateCreateInfo VulkanPipeline::GetDepthStencilState(const DepthState& aDepthState)
	{
		VkPipelineDepthStencilStateCreateInfo depthStencil
		{
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			nullptr,
			0,
			aDepthState.Test,
			aDepthState.Write,
			Utils::GetVulkanCompareOp(aDepthState.CompareOp),
			VK_FALSE,
			aDepthState.StencilState.Enabled,
			GetStencilOpState(aDepthState.StencilState.Front),
			GetStencilOpState(aDepthState.StencilState.Back),
			0.0f,
			1.0f
		};

		return depthStencil;
	}

	VkStencilOpState VulkanPipeline::GetStencilOpState(const StencilFaceState& aStencilState)
	{
		VkStencilOpState stencilState
		{
			Utils::GetVulkanStencilOp(aStencilState.FailOp),
			Utils::GetVulkanStencilOp(aStencilState.PassOp),
			Utils::GetVulkanStencilOp(aStencilState.DepthFailOp),
			Utils::GetVulkanCompareOp(aStencilState.CompareOp),
			aStencilState.CompareMask,
			aStencilState.WriteMask,
			aStencilState.Reference
		};

		return stencilState;
	}
	
	VkPipelineRasterizationStateCreateInfo VulkanPipeline::GetRasterizationState(const RasterState& aRasterState)
	{
		VkPipelineRasterizationStateCreateInfo rasterizer
		{
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			nullptr,
			0,
			aRasterState.DepthClamp,
			VK_FALSE,
			Utils::GetVulkanFillMode(aRasterState.Fill),
			Utils::GetVulkanCullMode(aRasterState.Cull),
			Utils::GetVulkanFrontFace(aRasterState.FrontFace),
			aRasterState.DepthBias,
			0.0f,
			0.0f,
			0.0f,
			1.0f
		};

		return rasterizer;
	}

	VkPipelineColorBlendAttachmentState VulkanPipeline::GetBlendState(const BlendState& aBlendState)
	{
		VkPipelineColorBlendAttachmentState colorBlendState
		{
			aBlendState.Enabled,
			Utils::GetVulkanBlendFactor(aBlendState.SrcColor),
			Utils::GetVulkanBlendFactor(aBlendState.DstColor),
			Utils::GetVulkanBlendOp(aBlendState.ColorOp),
			Utils::GetVulkanBlendFactor(aBlendState.SrcAlpha),
			Utils::GetVulkanBlendFactor(aBlendState.DstAlpha),
			Utils::GetVulkanBlendOp(aBlendState.AlphaOp),
			Utils::GetVulkanColorMask(aBlendState.ColorMask)
		};

		return colorBlendState;
	}
}