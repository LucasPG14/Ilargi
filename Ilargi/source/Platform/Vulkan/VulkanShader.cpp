#include "ilargipch.h"

#include "VulkanShader.h"
#include "VulkanContext.h"

#include <shaderc/shaderc.hpp>
#include <include/spirv_glsl.hpp>
#include <include/spirv_glsl.hpp>

namespace Ilargi
{
	namespace Utils
	{
		const VkShaderStageFlagBits GetShaderStageFromString(const std::string& type)
		{
			if (type == "vertex")	return VK_SHADER_STAGE_VERTEX_BIT;
			if (type == "vert") return VK_SHADER_STAGE_VERTEX_BIT;
			if (type == "fragment") return VK_SHADER_STAGE_FRAGMENT_BIT;
			if (type == "frag") return VK_SHADER_STAGE_FRAGMENT_BIT;

			ILG_ASSERT(nullptr, "Shader stage type not supported");
			return VK_SHADER_STAGE_VERTEX_BIT;
		}

		const std::string ShaderStageToString(VkShaderStageFlagBits stage)
		{
			switch (stage)
			{
			case VK_SHADER_STAGE_VERTEX_BIT: return "Vertex Shader";
			case VK_SHADER_STAGE_FRAGMENT_BIT: return "Fragment Shader";
			}

			ILG_ASSERT(nullptr, "Shader stage type not supported");
			return "";
		}

		const std::string ReadFile(const std::string& path)
		{
			std::string result;
			std::ifstream in(path, std::ios::in | std::ios::binary);
			if (in)
			{
				in.seekg(0, std::ios::end);
				result.resize(in.tellg());
				in.seekg(0, std::ios::beg);
				in.read(&result[0], result.size());
				in.close();
			}
			else ILG_CORE_ERROR("Couldn't open file '{0}'", path);

			return result;
		}

		const std::vector<uint32_t> ReadCacheFile(const std::string& path)
		{
			std::vector<uint32_t> result;
			std::ifstream in(path, std::ios::in | std::ios::binary);
			if (in)
			{
				in.seekg(0, std::ios::end);
				result.resize(in.tellg() / sizeof(uint32_t));
				in.seekg(0, std::ios::beg);
				in.read((char*)&result[0], result.size() * sizeof(uint32_t));
				in.close();
			}
			else ILG_CORE_ERROR("Couldn't open file '{0}'", path);

			return result;
		}

		const shaderc_shader_kind GLShaderStageToShaderC(VkShaderStageFlagBits stage)
		{
			switch (stage)
			{
			case VK_SHADER_STAGE_VERTEX_BIT:   return shaderc_glsl_vertex_shader;
			case VK_SHADER_STAGE_FRAGMENT_BIT: return shaderc_glsl_fragment_shader;
			}

			ILG_ASSERT(false, "");
			return (shaderc_shader_kind)0;
		}

		const std::filesystem::path GetCacheExtension(VkShaderStageFlagBits stage)
		{
			switch (stage)
			{
			case VK_SHADER_STAGE_VERTEX_BIT:   return "_cache_vert.spv";
			case VK_SHADER_STAGE_FRAGMENT_BIT: return "_cache_frag.spv";
			}

			ILG_ASSERT(false, "");
			return "";
		}

		const std::filesystem::path GetShaderCacheDirectory()
		{
			return "Cache/vulkan/shaders/";
		}

		void CreateShaderCacheDirectory()
		{
			const std::filesystem::path& cacheDirectory = GetShaderCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}
	}

	VulkanShader::VulkanShader(std::string_view aFilepath) : mFilepath(aFilepath), mName(std::filesystem::path(aFilepath).stem().string())
	{	
		ILG_PROFILE_FUNC;

		Utils::CreateShaderCacheDirectory();
		const auto& directory = Utils::GetShaderCacheDirectory();

		mSetBindingMap.fill({false, false, false, false, false, false, false, false});

		auto device = VulkanContext::GetLogicalDevice();

		auto nonCacheFileTime = std::filesystem::last_write_time(aFilepath);
		auto shaderCacheFile = mName + "_cache_";

		for (const auto& file : std::filesystem::recursive_directory_iterator(directory))
		{
			const auto& filename = file.path().stem().string();
			std::regex pattern(shaderCacheFile, std::regex_constants::icase);
			if (!std::regex_search(filename, pattern))
				continue;

			if (std::filesystem::last_write_time(file.path()) < nonCacheFileTime)
				break;

			std::string typeStr = filename.substr(filename.find_last_of("_") + 1);

			auto result = Utils::ReadCacheFile(file.path().string());
			CreateShaderModule(Utils::GetShaderStageFromString(typeStr), result);
		}

		if (mShaders.empty())
			ProcessShader();

		if (!mDescriptorSetBindings.empty())
		{
			uint32_t size = (--mDescriptorSetBindings.end())->first + 1;
			mDescriptorSetLayouts.resize(size);
			for (uint32_t setBindingIndex { 0 }; setBindingIndex < size; ++setBindingIndex)
			{
				if (mDescriptorSetBindings.find(setBindingIndex) != mDescriptorSetBindings.end())
				{
					VkDescriptorSetLayoutCreateInfo layoutInfo
					{
						VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,					// sType
						nullptr,																// pNext
						0,																		// flags
						static_cast<uint32_t>(mDescriptorSetBindings[setBindingIndex].size()),	// bindingCount
						mDescriptorSetBindings[setBindingIndex].data()							// pBindings
					};

					VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &mDescriptorSetLayouts[setBindingIndex]));
				}
			}
		}
	}
	
	VulkanShader::~VulkanShader()
	{
	}
	
	void VulkanShader::Destroy()
	{
		auto device = VulkanContext::GetLogicalDevice();

		for (auto& [stage, module] : mShaders)
		{
			vkDestroyShaderModule(device, module, nullptr);
		}

		for (auto& descriptorSetLayout : mDescriptorSetLayouts)
		{
			vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
		}

		mShaders.clear();
	}

	void VulkanShader::AllocateDescriptorSet(uint32_t aIndex, VkDescriptorSet& aDescriptorSet)
	{
		ILG_ASSERT(aIndex < mDescriptorSetLayouts.size(), "This descriptor set does not exist");

		auto device = VulkanContext::GetLogicalDevice();

		VkDescriptorSetAllocateInfo allocInfo
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,		// sType
			nullptr,											// pNext
			VulkanContext::GetDescriptorPool(),					// descriptorPool
			1,													// descriptorSetCount
			&mDescriptorSetLayouts[aIndex]						// pSetLayouts
		};

		VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &aDescriptorSet));
	}
	
	void VulkanShader::ProcessShader()
	{
		auto device = VulkanContext::GetLogicalDevice();

		std::string code = Utils::ReadFile(mFilepath.data());

		const char* type = "#type";
		size_t typeLength = strlen(type);
		size_t pos = code.find(type, 0);

		while (pos != std::string::npos)
		{
			size_t eol = code.find_first_of("\r\n", pos);
			ILG_ASSERT(eol != std::string::npos, "Syntax Error");
			size_t begin = pos + typeLength + 1;
			std::string shader = code.substr(begin, eol - begin);

			ILG_ASSERT(shader == "vertex" || shader == "fragment", "Invalid Shader Type");

			size_t nextLinePosition = code.find_first_not_of("\r\n", eol);
			pos = code.find(type, nextLinePosition);
			
			std::string finalShaderCode = code.substr(nextLinePosition, pos - (nextLinePosition == std::string::npos ? code.size() - 1 : nextLinePosition));
			
			VkShaderStageFlagBits stage = Utils::GetShaderStageFromString(shader.data());
			auto result = ConvertToSpirV(stage, finalShaderCode);

			std::filesystem::path filename = mFilepath;
			std::filesystem::path cacheFile = Utils::GetShaderCacheDirectory();
			cacheFile += filename.stem();
			cacheFile += Utils::GetCacheExtension(stage);

			// Saving to cache file
			std::ofstream file(cacheFile, std::ios::out | std::ios::binary);

			if (file.is_open())
			{
				file.write((char*)result.data(), result.size() * sizeof(uint32_t));
				file.flush();
				file.close();
			}

			CreateShaderModule(stage, result);
		}
	}

	void VulkanShader::CreateShaderModule(VkShaderStageFlagBits aStage, const std::vector<uint32_t>& aCode)
	{
		auto device = VulkanContext::GetLogicalDevice();

		VkShaderModuleCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,	// sType
			nullptr,										// pNext
			0,												// flags
			4 * aCode.size(),								// codeSize
			aCode.data()									// pCode
		};

		VkShaderModule shaderModule = nullptr;
		VK_CHECK_RESULT(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule));

		ILG_CORE_TRACE("VulkanShader::Reflect - {0} {1}", Utils::ShaderStageToString(aStage), mFilepath);

		ReflectShader(aStage, aCode);

		mShaders.push_back({ aStage, shaderModule });
	}
	
	const std::vector<uint32_t> VulkanShader::ConvertToSpirV(VkShaderStageFlagBits stage, const std::string_view& code) const
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
		options.SetGenerateDebugInfo();
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(code.data(), Utils::GLShaderStageToShaderC(stage), mFilepath.c_str(), options);
		
		if (module.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			ILG_CORE_ERROR("{0}", module.GetErrorMessage().c_str());
			ILG_ASSERT(false, "");
		}

		return std::vector<uint32_t>(module.cbegin(), module.cend());
	}
	
	void VulkanShader::ReflectShader(VkShaderStageFlags aStage, const std::vector<uint32_t>& aCode)
	{
		spirv_cross::Compiler compiler(aCode);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		// Reflecting push constants
		
		const auto& constants = resources.push_constant_buffers;
		for (const auto& pushConstant : constants)
		{
			const auto& type = compiler.get_type(pushConstant.base_type_id);
			uint32_t size = static_cast<uint32_t>(compiler.get_declared_struct_size(type));
			uint32_t binding = compiler.get_decoration(pushConstant.id, spv::DecorationBinding);
			uint32_t membersCount = static_cast<uint32_t>(type.member_types.size());

			ILG_CORE_TRACE("Push Constant: {0}", compiler.get_name(pushConstant.base_type_id));
			ILG_CORE_TRACE("	Size: {0}", size);
			ILG_CORE_TRACE("	Binding: {0}", binding);
			ILG_CORE_TRACE("	Members: {0}", membersCount);

			VkPushConstantRange pushConstant = { aStage, 0, size };
			
			mPushConstants.push_back(pushConstant);
		}

		// Reflecting uniform buffers
		const auto& resUniformBuffers = resources.uniform_buffers;
		for (const auto& uniformBuffer : resUniformBuffers)
		{
			const auto& type = compiler.get_type(uniformBuffer.base_type_id);
			uint32_t size = static_cast<uint32_t>(compiler.get_declared_struct_size(type));
			uint32_t binding = compiler.get_decoration(uniformBuffer.id, spv::DecorationBinding);
			uint32_t set = compiler.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet);
			uint32_t membersCount = static_cast<uint32_t>(type.member_types.size());

			ILG_CORE_TRACE("Uniform Buffer: {0}", uniformBuffer.name.c_str());
			ILG_CORE_TRACE("	Size: {0}", size);
			ILG_CORE_TRACE("	Set: {0}", set);
			ILG_CORE_TRACE("	Binding: {0}", binding);
			ILG_CORE_TRACE("	Members: {0}", membersCount);

			if (!mSetBindingMap[set][binding])
			{
				VkDescriptorSetLayoutBinding layoutBinding
				{
					binding,													// binding
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,							// descriptorType
					1,															// descriptorCount
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,	// stageFlags
					nullptr														// pImmutableSamplers
				};

				mSetBindingMap[set][binding] = true;
				mDescriptorSetBindings[set].push_back(layoutBinding);
			}
		}

		// Reflecting sampled images
		const auto& sampledImages = resources.sampled_images;
		for (const auto& sampledImage : sampledImages)
		{
			const auto& type = compiler.get_type(sampledImage.base_type_id);
			uint32_t binding = compiler.get_decoration(sampledImage.id, spv::DecorationBinding);
			uint32_t set = compiler.get_decoration(sampledImage.id, spv::DecorationDescriptorSet);

			ILG_CORE_TRACE("Sampler2D: {0}", sampledImage.name.c_str());
			ILG_CORE_TRACE("	Binding: {0}", binding);
			ILG_CORE_TRACE("	Descriptor Set: {0}", set);

			VkDescriptorSetLayoutBinding layoutBinding
			{
				binding,													// binding
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,					// descriptorType
				1,															// descriptorCount
				aStage,														// stageFlags
				nullptr														// pImmutableSamplers
			};

			mDescriptorSetBindings[set].push_back(layoutBinding);
		}

		// Reflecting separate images
		const auto& sepImages = resources.separate_images;
		for (const auto& separateImage : sepImages)
		{
			const auto& type = compiler.get_type(separateImage.base_type_id);
			uint32_t size = static_cast<uint32_t>(compiler.get_declared_struct_size(type));
			uint32_t binding = compiler.get_decoration(separateImage.id, spv::DecorationBinding);
			uint32_t membersCount = static_cast<uint32_t>(type.member_types.size());

			ILG_CORE_TRACE("Uniform Buffer: {0}", separateImage.name.c_str());
			ILG_CORE_TRACE("	Size: {0}", size);
			ILG_CORE_TRACE("	Binding: {0}", binding);
			ILG_CORE_TRACE("	Members: {0}", membersCount);
		}

		// Reflecting separate images
		const auto& sepSamplers = resources.separate_samplers;
		for (const auto& separateSampler : sepSamplers)
		{
			const auto& type = compiler.get_type(separateSampler.base_type_id);
			uint32_t size = static_cast<uint32_t>(compiler.get_declared_struct_size(type));
			uint32_t binding = compiler.get_decoration(separateSampler.id, spv::DecorationBinding);
			uint32_t membersCount = static_cast<uint32_t>(type.member_types.size());

			ILG_CORE_TRACE("Uniform Buffer: {0}", separateSampler.name.c_str());
			ILG_CORE_TRACE("	Size: {0}", size);
			ILG_CORE_TRACE("	Binding: {0}", binding);
			ILG_CORE_TRACE("	Members: {0}", membersCount);
		}
	}
}