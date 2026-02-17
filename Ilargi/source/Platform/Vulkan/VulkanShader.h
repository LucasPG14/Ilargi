#pragma once

#include "Renderer/IShader.h"
#include "Renderer/IGraphicsPipeline.h"
#include "VulkanPipelineLayout.h"
#include <vulkan/vulkan.h>

namespace Ilargi
{
	struct VertexLayout;

	using ShadersMap = std::vector<std::pair<VkShaderStageFlagBits, VkShaderModule>>;

	class VulkanShader : public IShader
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

		const uint32_t GetVertexInputsCount() const override { return mVertexInputsCount; }

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
		* @brief Returns the vulkan pipeline layout.
		* @return Instance of the vulkan pipeline layout.
		*/
		[[nodiscard]] const VkPipelineLayout& GetPipelineLayout() const { return mPipelineLayout->GetPipelineLayout(); }

	private:
		/*
		* @brief Returns the directory of the vulkan shader cache files.
		* @return The directory of the vulkan shader cache files.
		*/
		[[nodiscard]] constexpr const char* GetShaderCacheDirectory() const { return "cache/vulkan/"; }

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
		std::string mFilepath; // The shader filepath.
		std::string mName; // The name of the shader.

		uint32_t mVertexInputsCount;
		ShadersMap mShaders; // Map with the shaders based on their type(vertex, fragment, compute...).
		
		std::shared_ptr<VulkanPipelineLayout> mPipelineLayout; // Instance of the pipeline layout.
		PipelineLayoutProperties mPipelineLayoutProperties; // The pipeline layout properties.
	};
}