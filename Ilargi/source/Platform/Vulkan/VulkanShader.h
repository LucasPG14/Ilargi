#pragma once

#include "Renderer/Shader.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(std::string_view path);
		virtual ~VulkanShader();

		void Destroy();

		const std::vector<std::pair<VkShaderStageFlagBits, VkShaderModule>> GetShaders() const { return shaders; }

		const std::vector<VkPushConstantRange>& GetPushConstants() const { return pushConstants; }

	private:
		VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);

		const char* GetShaderCacheDirectory() { return "cache/vulkan/"; }

		void ProcessShader(std::string code);

		const std::vector<uint32_t> ConvertToSpirV(VkShaderStageFlagBits stage, std::string_view code);

		void ReflectShader(const std::vector<uint32_t>& code, VkShaderStageFlags stage);
	private:
		std::string filePath;

		std::vector<std::pair<VkShaderStageFlagBits, VkShaderModule>> shaders;

		std::vector<VkPushConstantRange> pushConstants;
	};
}