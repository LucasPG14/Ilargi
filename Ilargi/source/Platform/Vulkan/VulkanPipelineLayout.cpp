#include "ilargipch.h"
#include "VulkanPipelineLayout.h"

#include "VulkanGraphicsContext.h"
#include "VulkanDescriptorSetLayout.h"
#include "Renderer/Renderer.h"

namespace Ilargi
{
	VulkanPipelineLayout::VulkanPipelineLayout(const PipelineLayoutProperties& aPipelineLayoutProperties)
	{
		const auto& device{ VulkanGraphicsContext::GetLogicalDevice() };
		
		mDescriptorSetLayouts.reserve(aPipelineLayoutProperties.DescriptorSetLayoutsProperties.size());

		for (const auto& descriptorLayout : aPipelineLayoutProperties.DescriptorSetLayoutsProperties)
		{
			const auto& vulkanDescriptorSetLayout{ Renderer::GetDescriptorSetLayout(descriptorLayout)->As<VulkanDescriptorSetLayout>() };
			mDescriptorSetLayouts.emplace_back(vulkanDescriptorSetLayout->GetLayout());
		}

		std::vector<VkPushConstantRange> pushConstantRanges;
		for (const auto& pushConstant : aPipelineLayoutProperties.PushConstantRanges)
		{
			// TODO: Store only the stage where the binding is used.
			pushConstantRanges.emplace_back(VK_SHADER_STAGE_ALL_GRAPHICS, pushConstant.Offset, pushConstant.Size);
		}

		VkPipelineLayoutCreateInfo pipelineLayoutInfo
		{
			.sType {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO},
			.pNext {nullptr},
			.flags {0U},
			.setLayoutCount {static_cast<uint32_t>(mDescriptorSetLayouts.size())},
			.pSetLayouts {mDescriptorSetLayouts.data()},
			.pushConstantRangeCount {static_cast<uint32_t>(pushConstantRanges.size())},
			.pPushConstantRanges {pushConstantRanges.empty() ? nullptr : pushConstantRanges.data()}
		};

		VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &mPipelineLayout));
	}
	
	VulkanPipelineLayout::~VulkanPipelineLayout()
	{
		const auto& device{ VulkanGraphicsContext::GetLogicalDevice() };

		vkDestroyPipelineLayout(device, mPipelineLayout, nullptr);
	}
}