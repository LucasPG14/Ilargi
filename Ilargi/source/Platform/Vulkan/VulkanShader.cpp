#include "ilargipch.h"

#include "VulkanShader.h"
#include "VulkanContext.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Ilargi
{
	static std::vector<char> readFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	VulkanShader::VulkanShader(std::string_view vert, std::string_view frag) 
		: vertexShader(VK_NULL_HANDLE), fragmentShader(VK_NULL_HANDLE)
	{
		auto device = VulkanContext::GetLogicalDevice();

		auto vertShaderCode = readFile(vert.data());
		auto fragShaderCode = readFile(frag.data());

		vertexShader = CreateShaderModule(device, vertShaderCode);
		fragmentShader = CreateShaderModule(device, fragShaderCode);
	}
	
	VulkanShader::~VulkanShader()
	{
	}
	
	void VulkanShader::Destroy()
	{
		auto device = VulkanContext::GetLogicalDevice();

		vkDestroyShaderModule(device, vertexShader, nullptr);
		vkDestroyShaderModule(device, fragmentShader, nullptr);
	}
	
	VkShaderModule VulkanShader::CreateShaderModule(VkDevice device, const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule = nullptr;
		VK_CHECK_RESULT(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule));

		return shaderModule;
	}
}