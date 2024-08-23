#include "ilargipch.h"

#include "VulkanPipeline.h"
#include "Renderer/Renderer.h"
#include "Resources/Material.h"
#include "VulkanContext.h"
#include "VulkanRenderPass.h"
#include "VulkanCommandBuffer.h"
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
			case ShaderDataType::FLOAT:		return VK_FORMAT_R32_SFLOAT;
			case ShaderDataType::FLOAT2:	return VK_FORMAT_R32G32_SFLOAT;
			case ShaderDataType::FLOAT3:	return VK_FORMAT_R32G32B32_SFLOAT;
			case ShaderDataType::FLOAT4:	return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ShaderDataType::INT:		return VK_FORMAT_R32_SINT;
			case ShaderDataType::INT2:		return VK_FORMAT_R32G32_SINT;
			case ShaderDataType::INT3:		return VK_FORMAT_R32G32B32_SINT;
			case ShaderDataType::INT4:		return VK_FORMAT_R32G32B32A32_SINT;
			}

			ILG_ASSERT(nullptr, "Vk format not found for Shader Data Type")
			return VkFormat();
		}
	}

	VulkanPipeline::VulkanPipeline(const PipelineProperties& aProperties) 
		: mProperties(aProperties), mPipeline(VK_NULL_HANDLE), mPipelineLayout(VK_NULL_HANDLE), mDescriptorSetLayout(VK_NULL_HANDLE)
	{
	}
	
	VulkanPipeline::~VulkanPipeline()
	{
	}

	void VulkanPipeline::Init(VkRenderPass aRenderPass, const std::vector<ImageFormat>& aFormats)
	{
		auto device = VulkanContext::GetLogicalDevice();

		// Pipeline cache
		VkPipelineCache pipelineCache = VK_NULL_HANDLE;

		VkPipelineCacheCreateInfo pipelineCacheInfo = {};
		pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

		Buffer buffer = FileSystem::ReadBinaryFile(std::filesystem::path("cache/vulkan/pipelines/" + mProperties.name + ".pipe"));

		if (buffer.size > 0)
		{
			pipelineCacheInfo.initialDataSize = buffer.size;
			pipelineCacheInfo.pInitialData = buffer.data;
		}
		VK_CHECK_RESULT(vkCreatePipelineCache(device, &pipelineCacheInfo, nullptr, &pipelineCache));

		VkGraphicsPipelineCreateInfo pipelineInfo = {};

		auto shader = std::static_pointer_cast<VulkanShader>(mProperties.shader);

		// Creating the pipeline layout
		{
			VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(shader->GetDescriptorSetLayout().size());
			pipelineLayoutInfo.pSetLayouts = shader->GetDescriptorSetLayout().data();
			pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(shader->GetPushConstants().size());
			pipelineLayoutInfo.pPushConstantRanges = shader->GetPushConstants().data();

			VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &mPipelineLayout));
		}

		const auto& shaders = shader->GetShaders();
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		for (auto&& [type, shaderModule] : shaders)
		{
			VkPipelineShaderStageCreateInfo shaderStageInfo = {};
			shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageInfo.stage = type;
			shaderStageInfo.module = shaderModule;
			shaderStageInfo.pName = "main";

			shaderStages.push_back(shaderStageInfo);
		}

		// TODO: Gives an error when layout is empty
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = mProperties.layout.GetStride();
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		const auto& elements = mProperties.layout.GetElements();
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(elements.size());

		int i = 0;
		for (auto& attributeDescription : attributeDescriptions)
		{
			attributeDescription.binding = 0;
			attributeDescription.location = i;
			attributeDescription.format = Utils::GetVkFormatFromShaderDataType(elements[i].type);
			attributeDescription.offset = elements[i].offset;

			i++;
		}

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = attributeDescriptions.size() == 0 ? 0 : 1;
		vertexInputInfo.pVertexBindingDescriptions = attributeDescriptions.size() == 0 ? VK_NULL_HANDLE : &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		uint32_t width = 1080;
		uint32_t height = 720;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)width;
		viewport.height = (float)height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = { width, height };

		std::vector<VkDynamicState> dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;

		for (int i = 0; i < aFormats.size(); ++i)
		{
			if (Utils::IsDepth(aFormats[i]))
				continue;

			VkPipelineColorBlendAttachmentState& colorBlendAttachment = colorBlendAttachments.emplace_back();
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable = VK_TRUE;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		}

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
		colorBlending.pAttachments = colorBlendAttachments.data();
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		if (mProperties.depth)
		{
			VkPipelineDepthStencilStateCreateInfo depthStencil = {};
			depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencil.depthTestEnable = VK_TRUE;
			depthStencil.depthWriteEnable = VK_TRUE;
			depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
			depthStencil.depthBoundsTestEnable = VK_FALSE;
			depthStencil.minDepthBounds = 0.0f;
			depthStencil.maxDepthBounds = 1.0f;
			depthStencil.stencilTestEnable = VK_FALSE;
			depthStencil.front = {};
			depthStencil.back = {};
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

		pipelineInfo.layout = mPipelineLayout;

		pipelineInfo.renderPass = aRenderPass;
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineInfo, nullptr, &mPipeline));

		if (buffer.size == 0)
		{
			VK_CHECK_RESULT(vkGetPipelineCacheData(device, pipelineCache, &buffer.size, nullptr));

			buffer.data = new char[buffer.size];
			VK_CHECK_RESULT(vkGetPipelineCacheData(device, pipelineCache, &buffer.size, buffer.data));

			FileSystem::WriteBinaryFile(std::filesystem::path("cache/vulkan/pipelines/" + mProperties.name + ".pipe"), buffer);
		}
		
		delete buffer.data;

		vkDestroyPipelineCache(device, pipelineCache, nullptr);
	}

	void VulkanPipeline::Destroy()
	{
		auto device = VulkanContext::GetLogicalDevice();

		mProperties.shader->Destroy();
		vkDestroyPipeline(device, mPipeline, nullptr);
		vkDestroyPipelineLayout(device, mPipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(device, mDescriptorSetLayout, nullptr);
	}

	void VulkanPipeline::PushConstants(const std::shared_ptr<CommandBuffer>& aCommandBuffer, uint32_t aOffset, uint32_t aSize, const void* aData) const
	{
		Renderer::Submit([this, aCommandBuffer, aOffset, aSize, aData]()
			{
				uint32_t currentFrame = Renderer::GetCurrentFrame();

				auto cmdBuffer = std::static_pointer_cast<VulkanCommandBuffer>(aCommandBuffer)->GetCurrentCommand(currentFrame);
				vkCmdPushConstants(cmdBuffer, mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, aOffset, aSize, aData);
			});
	}

	void VulkanPipeline::Bind(const std::shared_ptr<CommandBuffer>& aCommandBuffer) const
	{
		Renderer::Submit([this, aCommandBuffer]()
			{
				uint32_t currentFrame = Renderer::GetCurrentFrame();

				auto cmdBuffer = std::static_pointer_cast<VulkanCommandBuffer>(aCommandBuffer)->GetCurrentCommand(currentFrame);
				vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
			});
	}
	
	void VulkanPipeline::BindDescriptorSet(const std::shared_ptr<CommandBuffer>& aCommandBuffer, std::shared_ptr<Material> aMaterial) const
	{
		Renderer::Submit([this, aCommandBuffer, aMaterial]()
			{
				uint32_t currentFrame = Renderer::GetCurrentFrame();

				auto cmdBuffer = std::static_pointer_cast<VulkanCommandBuffer>(aCommandBuffer)->GetCurrentCommand(currentFrame);
				std::vector<VkDescriptorSet> descriptorSets = { (VkDescriptorSet)aMaterial->GetDescriptorSet() };
				vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
			});
	}
}