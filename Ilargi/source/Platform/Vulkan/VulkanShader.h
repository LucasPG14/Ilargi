#pragma once

#include "Renderer/Shader.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	using ShadersMap = std::vector<std::pair<VkShaderStageFlagBits, VkShaderModule>>;

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(std::string_view aFilepath);
		virtual ~VulkanShader();

		void Destroy();

		void AllocateDescriptorSet(uint32_t aIndex, VkDescriptorSet& aDescriptorSet);

		const ShadersMap& GetShaders() const { return mShaders; }

		const std::vector<VkPushConstantRange>& GetPushConstants() const { return mPushConstants; }

		const std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayout() const { return mDescriptorSetLayouts; }

	private:
		const char* GetShaderCacheDirectory() const { return "cache/vulkan/"; }

		void ProcessShader();

		void CreateShaderModule(VkShaderStageFlagBits aStage, const std::vector<uint32_t>& aCode);

		const std::vector<uint32_t> ConvertToSpirV(VkShaderStageFlagBits aStage, const std::string_view& aCode) const;

		void ReflectShader(VkShaderStageFlags aStage, const std::vector<uint32_t>& aCode);
	
	private:
		std::string mFilepath;
		std::string mName;

		ShadersMap mShaders;

		std::vector<VkPushConstantRange> mPushConstants;
		std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;

		std::map<int, std::vector<VkDescriptorSetLayoutBinding>> mDescriptorSetBindings;
	};
}