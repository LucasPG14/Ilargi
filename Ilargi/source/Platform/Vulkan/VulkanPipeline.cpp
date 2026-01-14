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

		VkShaderStageFlags GetShaderStages(const ShaderStage aShaderStage)
		{
			VkShaderStageFlags shaderStage{ 0x00000000 };
			if (aShaderStage & VERTEX_SHADER)
			{
				shaderStage |= VK_SHADER_STAGE_VERTEX_BIT;
			}
			if (aShaderStage & FRAGMENT_SHADER)
			{
				shaderStage |= VK_SHADER_STAGE_FRAGMENT_BIT;
			}

			return shaderStage;
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
	}

	VulkanPipeline::VulkanPipeline(const PipelineProperties& aProperties) 
		: mProperties(aProperties), mPipeline(VK_NULL_HANDLE), mPipelineLayout(VK_NULL_HANDLE), mDescriptorSetLayout(VK_NULL_HANDLE)
	{
		Utils::CreatePipelineCacheDirectory();
		const auto& renderPass{ mProperties.renderPass->As<VulkanRenderPass>() };
		Init(renderPass->GetRenderPass(), renderPass->GetProperties().formats);
	}
	
	VulkanPipeline::~VulkanPipeline()
	{
	}

	void VulkanPipeline::Init(VkRenderPass aRenderPass, const std::vector<ImageFormat>& aFormats)
	{
		auto device{ VulkanContext::GetLogicalDevice() };

		// Pipeline cache
		VkPipelineCache pipelineCache{ VK_NULL_HANDLE };

		VkPipelineCacheCreateInfo pipelineCacheInfo {};
		pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

		std::filesystem::path pipelineCacheFilepath{ (std::filesystem::path("cache/vulkan/pipelines/" + mProperties.name + ".pipe")) };
		BinaryReader reader(pipelineCacheFilepath);

		if (std::filesystem::exists(pipelineCacheFilepath))
		{
			pipelineCacheInfo.initialDataSize = reader.GetSize();
			
			char* cacheData = new char[pipelineCacheInfo.initialDataSize];
			reader.Read(cacheData, pipelineCacheInfo.initialDataSize);
			
			pipelineCacheInfo.pInitialData = cacheData;
			delete[] cacheData;
		}
		VK_CHECK_RESULT(vkCreatePipelineCache(device, &pipelineCacheInfo, nullptr, &pipelineCache));

		VkGraphicsPipelineCreateInfo pipelineInfo {};

		const auto& shader{ mProperties.shader->As<VulkanShader>() };

		// Creating the pipeline layout
		{
			VkPipelineLayoutCreateInfo pipelineLayoutInfo
			{
				VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,					// sType
				nullptr,														// pNext
				0,																// flags
				static_cast<uint32_t>(shader->GetDescriptorSetLayout().size()), // setLayoutCount
				shader->GetDescriptorSetLayout().data(),						// pSetLayouts
				static_cast<uint32_t>(shader->GetPushConstants().size()),		// pushConstantRangeCount
				shader->GetPushConstants().data()								// pPushConstantRanges
			};

			VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &mPipelineLayout));
		}

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
			0,								// binding
			mProperties.layout.GetStride(), // stride
			VK_VERTEX_INPUT_RATE_VERTEX		// inputRate
		};

		const auto& elements{ mProperties.layout.GetElements() };
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(mProperties.layoutUsage);

		int i{ 0 };
		for (auto& attributeDescription : attributeDescriptions)
		{
			attributeDescription.binding = 0;
			attributeDescription.location = i;
			attributeDescription.format = Utils::GetVkFormatFromShaderDataType(elements[i].type);
			attributeDescription.offset = elements[i].offset;

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

		VkPipelineRasterizationStateCreateInfo rasterizer
		{
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, // sType
			nullptr,													// pNext
			0,															// flags
			VK_FALSE,													// depthClampEnable
			VK_FALSE,													// rasterizerDiscardEnable
			VK_POLYGON_MODE_FILL,										// polygonMode
			VK_CULL_MODE_NONE,											// cullMode
			VK_FRONT_FACE_COUNTER_CLOCKWISE,							// frontFace
			VK_FALSE,													// depthBiasEnable
			0.0f,														// depthBiasConstantFactor
			0.0f,														// depthBiasClamp
			0.0f,														// depthBiasSlopeFactor
			1.0f														// lineWidth
		};

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

		for (uint32_t i { 0 }; i < aFormats.size(); ++i)
		{
			if (Utils::IsDepth(aFormats[i]))
				continue;

			VkPipelineColorBlendAttachmentState& colorBlendAttachment{ colorBlendAttachments.emplace_back() };
			colorBlendAttachment.colorWriteMask = mProperties.blend ? VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT : VK_COLOR_COMPONENT_R_BIT;
			colorBlendAttachment.blendEnable = mProperties.blend;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
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

		if (mProperties.writeDepth || mProperties.testDepth)
		{
			VkPipelineDepthStencilStateCreateInfo depthStencil
			{
				VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, // sType
				nullptr,													// pNext
				0,															// flags
				mProperties.testDepth,										// depthTestEnable
				mProperties.writeDepth,										// depthWriteEnable
				VK_COMPARE_OP_LESS,											// depthCompareOp
				VK_FALSE,													// depthBoundsTestEnable
				mProperties.hasStencil,										// stencilTestEnable
				{},															// front
				{},															// back
				0.0f,														// minDepthBounds
				1.0f														// maxDepthBounds
			};

			if (mProperties.hasStencil)
			{
				VkStencilOpState stencilWrite
				{
					VK_STENCIL_OP_KEEP,															//failOp
					mProperties.writeStencil ? VK_STENCIL_OP_REPLACE : VK_STENCIL_OP_KEEP,		//passOp
					VK_STENCIL_OP_KEEP,															//depthFailOp
					mProperties.writeStencil ? VK_COMPARE_OP_ALWAYS : VK_COMPARE_OP_NOT_EQUAL,	//compareOp
					0xFFU,																		//compareMask
					mProperties.writeStencil ? 0xFFU : 0x00U,									//writeMask
					1U																			//reference
				};

				depthStencil.front = stencilWrite;
				depthStencil.back = stencilWrite;
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

		pipelineInfo.layout = mPipelineLayout;

		pipelineInfo.renderPass = aRenderPass;
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineInfo, nullptr, &mPipeline));

		if (!std::filesystem::exists(pipelineCacheFilepath))
		{
			size_t pipelineSize;
			VK_CHECK_RESULT(vkGetPipelineCacheData(device, pipelineCache, &pipelineSize, nullptr));

			char* data = new char[pipelineSize];
			VK_CHECK_RESULT(vkGetPipelineCacheData(device, pipelineCache, &pipelineSize, data));

			BinaryWriter writer(pipelineCacheFilepath);
			writer.Write(data, pipelineSize);

			delete[] data;
		}

		vkDestroyPipelineCache(device, pipelineCache, nullptr);
	}

	void VulkanPipeline::Destroy()
	{
		auto device{ VulkanContext::GetLogicalDevice() };

		mProperties.shader->Destroy();
		vkDestroyPipeline(device, mPipeline, nullptr);
		vkDestroyPipelineLayout(device, mPipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(device, mDescriptorSetLayout, nullptr);
	}

	void VulkanPipeline::PushConstants(const std::shared_ptr<CommandBuffer>& aCommandBuffer, ShaderStage aShaderStage, uint32_t aOffset, uint32_t aSize, const void* aData) const
	{
		Renderer::Submit([this, aCommandBuffer, aShaderStage, aOffset, aSize, aData]()
			{
				const uint32_t currentFrame{ Renderer::GetCurrentFrame() };

				const VkCommandBuffer cmdBuffer{ aCommandBuffer->As<VulkanCommandBuffer>()->GetCurrentCommand(currentFrame) };
				vkCmdPushConstants(cmdBuffer, mPipelineLayout, VK_SHADER_STAGE_ALL_GRAPHICS, aOffset, aSize, aData);
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
				vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
			});
	}

	void VulkanPipeline::BindUniformBuffer(const std::shared_ptr<CommandBuffer>& aCommandBuffer, const std::shared_ptr<UniformBuffer>& aUniformBuffer, uint32_t aSetIndex) const
	{
		Renderer::Submit([this, aCommandBuffer, aUniformBuffer, aSetIndex]()
			{
				uint32_t currentFrame { Renderer::GetCurrentFrame() };

				const VkCommandBuffer cmdBuffer { aCommandBuffer->As<VulkanCommandBuffer>()->GetCurrentCommand(currentFrame) };
				std::vector<VkDescriptorSet> descriptorSets { aUniformBuffer->As<VulkanUniformBuffer>()->GetDescriptorSet() };
				vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, aSetIndex, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
			});
	}
}