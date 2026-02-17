#include "ilargipch.h"

#include "VulkanGraphicsPipeline.h"
#include "Renderer/Renderer.h"
#include "Resources/Material.h"
#include "VulkanGraphicsContext.h"
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
			case ColorMask::NONE:	return 0;
			case ColorMask::R:		return VK_COLOR_COMPONENT_R_BIT;
			case ColorMask::RG:		return VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT;
			case ColorMask::RGB:	return VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
			case ColorMask::RGBA:	return VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			}

			return VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		}
	}

	VulkanGraphicsPipeline::VulkanGraphicsPipeline(const GraphicsPipelineProperties& aProperties) 
		: mProperties(aProperties), mPipeline(VK_NULL_HANDLE)
	{
		Utils::CreatePipelineCacheDirectory();
		Init(aProperties.ColorFormats);
	}
	
	VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
	{
		const auto& device{ VulkanGraphicsContext::GetLogicalDevice() };

		vkDestroyPipeline(device, mPipeline, nullptr);
	}

	void VulkanGraphicsPipeline::Init(const std::vector<ImageFormat>& aFormats)
	{
		auto device{ VulkanGraphicsContext::GetLogicalDevice() };

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
				.sType {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO},
				.pNext {nullptr},
				.flags {0U},
				.stage {type},
				.module {shaderModule},
				.pName {"main"},
				.pSpecializationInfo {nullptr}
			};

			shaderStages.push_back(shaderStageInfo);
		}

		// TODO: Gives an error when layout is empty
		VkVertexInputBindingDescription bindingDescription
		{
			.binding {0U},
			.stride {mProperties.VertexLayout.GetStride()},
			.inputRate {VK_VERTEX_INPUT_RATE_VERTEX}
		};

		const auto& elements{ mProperties.VertexLayout.GetElements() };
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(shader->GetVertexInputsCount());

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
			.sType {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO},
			.pNext {nullptr},
			.flags {0U},
			.vertexBindingDescriptionCount {attributeSize == 0 ? 0U : 1U},
			.pVertexBindingDescriptions {attributeSize == 0 ? VK_NULL_HANDLE : &bindingDescription},
			.vertexAttributeDescriptionCount {attributeSize == 0 ? 0U : attributeSize},
			.pVertexAttributeDescriptions {attributeSize == 0 ? VK_NULL_HANDLE : attributeDescriptions.data()}
		};

		VkPipelineInputAssemblyStateCreateInfo inputAssembly
		{
			.sType {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO},
			.pNext {nullptr},
			.flags {0U},
			.topology {VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST},
			.primitiveRestartEnable {VK_FALSE}
		};

		uint32_t width{ 1080U };
		uint32_t height{ 720U };

		VkViewport viewport
		{
			.x {0.0f},
			.y {0.0f},
			.width {(float)width},
			.height {(float)height},
			.minDepth {0.0f},
			.maxDepth {1.0f}
		};

		VkRect2D scissor
		{
			.offset { 0, 0 },
			.extent { width, height }
		};

		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState
		{
			.sType {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO},
			.pNext {nullptr},
			.flags {0U},
			.dynamicStateCount {static_cast<uint32_t>(dynamicStates.size())},
			.pDynamicStates {dynamicStates.data()}
		};

		VkPipelineViewportStateCreateInfo viewportState
		{
			.sType {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO},
			.pNext {nullptr},
			.flags {0U},
			.viewportCount {1},
			.pViewports {&viewport},
			.scissorCount {1},
			.pScissors {&scissor}
		};

		if (mProperties.DepthState.Enabled)
		{
			VkPipelineDepthStencilStateCreateInfo depthStencil{ GetDepthStencilState(mProperties.DepthState) };

			pipelineInfo.pDepthStencilState = &depthStencil;
		}

		VkPipelineRasterizationStateCreateInfo rasterizer{ GetRasterizationState(mProperties.RasterState) };

		VkPipelineMultisampleStateCreateInfo multisampling
		{
			.sType {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO},
			.pNext {nullptr},
			.flags {0U},
			.rasterizationSamples {VK_SAMPLE_COUNT_1_BIT},
			.sampleShadingEnable {VK_FALSE},
			.minSampleShading {1.0f},
			.pSampleMask {nullptr},
			.alphaToCoverageEnable {VK_FALSE},
			.alphaToOneEnable {VK_FALSE}
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
			.sType {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO},
			.pNext {nullptr},
			.flags {0U},
			.logicOpEnable {VK_FALSE},
			.logicOp {VK_LOGIC_OP_COPY},
			.attachmentCount {static_cast<uint32_t>(colorBlendAttachments.size())},
			.pAttachments {colorBlendAttachments.data()},
			.blendConstants { 0.0f, 0.0f, 0.0f, 0.0f }
		};
		
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

		pipelineInfo.renderPass = Renderer::GetRenderPass({aFormats}).As<VulkanRenderPass>().GetRenderPass();
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

	void VulkanGraphicsPipeline::Destroy()
	{
		auto device{ VulkanGraphicsContext::GetLogicalDevice() };

		vkDestroyPipeline(device, mPipeline, nullptr);
	}
	
	VkPipelineDepthStencilStateCreateInfo VulkanGraphicsPipeline::GetDepthStencilState(const DepthState& aDepthState)
	{
		VkPipelineDepthStencilStateCreateInfo depthStencil
		{
			.sType {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO},
			.pNext {nullptr},
			.flags {0U},
			.depthTestEnable {aDepthState.Test},
			.depthWriteEnable {aDepthState.Write},
			.depthCompareOp {Utils::GetVulkanCompareOp(aDepthState.CompareOp)},
			.depthBoundsTestEnable {VK_FALSE},
			.stencilTestEnable {aDepthState.StencilState.Enabled},
			.front {GetStencilOpState(aDepthState.StencilState.Front)},
			.back {GetStencilOpState(aDepthState.StencilState.Back)},
			.minDepthBounds {0.0f},
			.maxDepthBounds {1.0f}
		};

		return depthStencil;
	}

	VkStencilOpState VulkanGraphicsPipeline::GetStencilOpState(const StencilFaceState& aStencilState)
	{
		VkStencilOpState stencilState
		{
			.failOp {Utils::GetVulkanStencilOp(aStencilState.FailOp)},
			.passOp {Utils::GetVulkanStencilOp(aStencilState.PassOp)},
			.depthFailOp {Utils::GetVulkanStencilOp(aStencilState.DepthFailOp)},
			.compareOp {Utils::GetVulkanCompareOp(aStencilState.CompareOp)},
			.compareMask {aStencilState.CompareMask},
			.writeMask {aStencilState.WriteMask},
			.reference {aStencilState.Reference}
		};

		return stencilState;
	}
	
	VkPipelineRasterizationStateCreateInfo VulkanGraphicsPipeline::GetRasterizationState(const RasterState& aRasterState)
	{
		VkPipelineRasterizationStateCreateInfo rasterizer
		{
			.sType {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO},
			.pNext {nullptr},
			.flags {0U},
			.depthClampEnable {aRasterState.DepthClamp},
			.rasterizerDiscardEnable {VK_FALSE},
			.polygonMode {Utils::GetVulkanFillMode(aRasterState.Fill)},
			.cullMode {Utils::GetVulkanCullMode(aRasterState.Cull)},
			.frontFace {Utils::GetVulkanFrontFace(aRasterState.FrontFace)},
			.depthBiasEnable {aRasterState.DepthBias},
			.depthBiasConstantFactor {0.0f},
			.depthBiasClamp {0.0f},
			.depthBiasSlopeFactor {0.0f},
			.lineWidth {1.0f}
		};

		return rasterizer;
	}

	VkPipelineColorBlendAttachmentState VulkanGraphicsPipeline::GetBlendState(const BlendState& aBlendState)
	{
		VkPipelineColorBlendAttachmentState colorBlendState
		{
			.blendEnable {aBlendState.Enabled},
			.srcColorBlendFactor {Utils::GetVulkanBlendFactor(aBlendState.SrcColor)},
			.dstColorBlendFactor {Utils::GetVulkanBlendFactor(aBlendState.DstColor)},
			.colorBlendOp {Utils::GetVulkanBlendOp(aBlendState.ColorOp)},
			.srcAlphaBlendFactor {Utils::GetVulkanBlendFactor(aBlendState.SrcAlpha)},
			.dstAlphaBlendFactor {Utils::GetVulkanBlendFactor(aBlendState.DstAlpha)},
			.alphaBlendOp {Utils::GetVulkanBlendOp(aBlendState.AlphaOp)},
			.colorWriteMask {Utils::GetVulkanColorMask(aBlendState.ColorMask)}
		};

		return colorBlendState;
	}
}