#pragma once

#include "Renderer/Shader.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	using ShadersMap = std::vector<std::pair<VkShaderStageFlagBits, VkShaderModule>>;

	class VulkanShader : public Shader
	{
	public:
		/*
		* @brief Constructor.
		* @param aFilepath The path of the shader file.
		*/
		VulkanShader(std::string_view aFilepath);
		
		/*
		* @brief Destructor.
		*/
		virtual ~VulkanShader();

		/*
		* @copydoc Shader::Destroy()
		*/
		void Destroy() override;

		/*
		* @brief Allocates the passed descriptor set.
		* @param aIndex The set index to allocate the descriptor set.
		* @param aDescriptorSet The descriptor set to allocate.
		*/
		void AllocateDescriptorSet(uint32_t aIndex, VkDescriptorSet& aDescriptorSet);

		/*
		* @copydoc Shader::GetName()
		*/
		[[nodiscard]] const std::string& GetName() const override { return mName; }

		/*
		* @brief Returns the map of shaders.
		* @return The shaders map.
		*/
		[[nodiscard]] const ShadersMap& GetShaders() const { return mShaders; }

		/*
		* @brief Returns the push constants container of the shader.
		* @return The push constants container.
		*/
		[[nodiscard]] const std::vector<VkPushConstantRange>& GetPushConstants() const { return mPushConstants; }

		/*
		* @brief Returns the descriptor set layout container of the shader.
		* @return The descriptor set layout container.
		*/
		[[nodiscard]] const std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayout() const { return mDescriptorSetLayouts; }

	private:
		/*
		* @brief Returns the directory of the vulkan shader cache files.
		* @return The directory of the vulkan shader cache files.
		*/
		[[nodiscard]] const char* GetShaderCacheDirectory() const { return "cache/vulkan/"; }

		/*
		* @brief Reads the shader file and process it.
		*/
		void ProcessShader();

		/*
		* @brief Creates the vulkan shader module.
		* @param aStage Indicates the type of shader.
		* @param aCode The shader code.
		*/
		void CreateShaderModule(VkShaderStageFlagBits aStage, const std::vector<uint32_t>& aCode);

		/*
		* @brief Converts the shader code to the binary format used in Vulkan.
		* @param aStage Indicates the type of shader.
		* @param aCode The shader code.
		* @return The code of the shader in SpirV format.
		*/
		const std::vector<uint32_t> ConvertToSpirV(VkShaderStageFlagBits aStage, const std::string_view& aCode) const;

		/*
		* @brief Stores the information of descriptors sets and push constants of the shader.
		* @param aStage Indicates the type of shader.
		* @param aCode The shader code.
		*/
		void ReflectShader(VkShaderStageFlags aStage, const std::vector<uint32_t>& aCode);
	
	private:
		std::array<std::array<bool, 8>, 8> mSetBindingMap; // Descriptor sets of the shader.
		std::string mFilepath; // The shader filepath.
		std::string mName; // The name of the shader.

		ShadersMap mShaders; // Map with the shaders based on their type(vertex, fragment, compute...).
		std::vector<VkPushConstantRange> mPushConstants; // Container with the push constants of the shader.
		std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts; // Container of the descriptor sets layouts.

		std::map<int, std::vector<VkDescriptorSetLayoutBinding>> mDescriptorSetBindings; // Map of the descriptor sets bindings.
	};
}