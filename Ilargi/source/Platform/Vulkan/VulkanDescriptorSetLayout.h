#pragma once

#include "Renderer/IDescriptorSetLayout.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanDescriptorSetLayout : public IDescriptorSetLayout
	{
	public:
		/*
		* @brief Constructor.
		* @param aProperties The properties of the descriptor set layout.
		*/
		VulkanDescriptorSetLayout(const DescriptorSetLayoutProperties& aProperties);

		/*
		* @brief Destructor.
		*/
		virtual ~VulkanDescriptorSetLayout();

		/*
		* @brief Return the descriptor set layout.
		* @return Instance of the vulkan descriptor set layout.
		*/
		const VkDescriptorSetLayout& GetLayout() const { return mDescriptorSetLayout; }
	
	private:
		VkDescriptorSetLayout mDescriptorSetLayout; // Instance of the vulkan descriptor set layout.
	};
}