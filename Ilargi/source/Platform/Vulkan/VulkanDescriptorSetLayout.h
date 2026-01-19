#pragma once

#include "Renderer/DescriptorSetLayout.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanDescriptorSetLayout : public DescriptorSetLayout
	{
	public:
		/*
		* @brief Constructor.
		* @param aProperties The properties of the descriptor set layout.
		*/
		VulkanDescriptorSetLayout(const DescriptorSetLayoutProperties& aProperties);

		/*
		* @brief Return the descriptor set layout.
		* @return Instance of the vulkan descriptor set layout.
		*/
		const VkDescriptorSetLayout& GetLayout() const { return mDescriptorSetLayout; }
	
	private:
		VkDescriptorSetLayout mDescriptorSetLayout; // Instance of the vulkan descriptor set layout.
	};
}