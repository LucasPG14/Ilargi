#include "ilargipch.h"

#include "VulkanDescriptorSetLayout.h"
#include "VulkanContext.h"

namespace Ilargi
{
	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const DescriptorSetLayoutProperties& aProperties) 
		: mDescriptorSetLayout(VK_NULL_HANDLE)
	{
		const auto& device{ VulkanContext::GetLogicalDevice() };

		std::vector<VkDescriptorSetLayoutBinding> vulkanBindings;
		vulkanBindings.reserve(aProperties.DescriptorBindings.size());

		for (const auto& binding : aProperties.DescriptorBindings)
		{
			VkDescriptorSetLayoutBinding& vulkanBinding{ vulkanBindings.emplace_back() };
			vulkanBinding.binding = binding.Binding;
			vulkanBinding.descriptorCount = 1;
			vulkanBinding.descriptorType = Utils::GetVulkanDescriptorType(binding.Type);
			vulkanBinding.pImmutableSamplers = nullptr;
			// TODO: Store only the stage where the binding is used.
			vulkanBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
		}
		
		VkDescriptorSetLayoutCreateInfo layoutInfoSet1
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,	// sType
			nullptr,												// pNext
			0,														// flags
			vulkanBindings.size(),										// bindingCount
			vulkanBindings.data()										// pBindings
		};

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &layoutInfoSet1, nullptr, &mDescriptorSetLayout));
	}
	
	VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
	{
		const auto& device{ VulkanContext::GetLogicalDevice() };

		vkDestroyDescriptorSetLayout(device, mDescriptorSetLayout, nullptr);
	}
}