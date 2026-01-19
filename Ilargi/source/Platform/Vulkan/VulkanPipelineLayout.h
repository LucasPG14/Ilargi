#pragma once

#include "Renderer/PipelineLayout.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanPipelineLayout : public PipelineLayout
	{
	public:
		/*
		* @brief Constructor.
		* @param aPipelineLayoutProperties The data to create the pipeline layout.
		*/
		VulkanPipelineLayout(const PipelineLayoutProperties& aPipelineLayoutProperties);

		/*
		* @brief Returns the vulkan pipeline layout.
		* @return Instance of the vulkan pipeline layout.
		*/
		[[nodiscard]] const VkPipelineLayout& GetPipelineLayout() const { return mPipelineLayout; }
		
		/*
		* @brief Returns the vulkan descriptor set layout on the given index.
		* @param aSetIndex The set index of the descriptor set layout needed
		* @return Instance of the vulkan descriptor set layout.
		*/
		[[nodiscard]] const VkDescriptorSetLayout& GetDescriptorSetLayout(uint8_t aSetIndex) const { return mDescriptorSetLayouts[aSetIndex]; }
		
		/*
		* @brief Returns the amount of descriptor set layouts.
		* @return The amount of descriptor set layouts.
		*/
		[[nodiscard]] const size_t GetDescriptorSetLayoutsCount() const { return mDescriptorSetLayouts.size(); }
	private:
		std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts; // Container of the vulkan descriptor set layouts.
		VkPipelineLayout mPipelineLayout; // Instance of the vulkan pipeline layout.
	};
}