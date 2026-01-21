#include "ilargipch.h"
#include "VulkanPipelineLayout.h"

#include "VulkanContext.h"
#include "VulkanDescriptorSetLayout.h"
#include "Renderer/Renderer.h"

namespace Ilargi
{
	VulkanPipelineLayout::VulkanPipelineLayout(const PipelineLayoutProperties& aPipelineLayoutProperties)
	{
		const auto& device{ VulkanContext::GetLogicalDevice() };
		
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
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,						// sType
			nullptr,															// pNext
			0,																	// flags
			mDescriptorSetLayouts.size(),										// setLayoutCount
			mDescriptorSetLayouts.data(),										// pSetLayouts
			pushConstantRanges.size(),											// pushConstantRangeCount
			pushConstantRanges.empty() ? nullptr : pushConstantRanges.data()	// pPushConstantRanges
		};

		VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &mPipelineLayout));
	}
	
	VulkanPipelineLayout::~VulkanPipelineLayout()
	{
		const auto& device{ VulkanContext::GetLogicalDevice() };

		vkDestroyPipelineLayout(device, mPipelineLayout, nullptr);
	}
}