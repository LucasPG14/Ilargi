#pragma once

#include "Renderer/Shader.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	using ShadersMap = std::vector<std::pair<VkShaderStageFlagBits, VkShaderModule>>;

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(std::string_view path);
		virtual ~VulkanShader();

		void Destroy();

		VkDescriptorSet AllocateDescriptorSet();

		const ShadersMap GetShaders() const { return shaders; }

		const std::vector<VkPushConstantRange>& GetPushConstants() const { return pushConstants; }

		const std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayout() const { return descriptorSetLayouts; }

	private:
		const char* GetShaderCacheDirectory() const { return "cache/vulkan/"; }

		void ProcessShader(std::string code);

		const std::vector<uint32_t> ConvertToSpirV(VkShaderStageFlagBits stage, std::string_view code) const;

		void ReflectShader(const std::vector<uint32_t>& code, VkShaderStageFlags stage);
	private:
		std::string filePath;
		std::string name;

		ShadersMap shaders;

		std::vector<VkPushConstantRange> pushConstants;
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
	};
}