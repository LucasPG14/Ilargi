#include "ilargipch.h"

#include "VulkanShader.h"
#include "VulkanContext.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Ilargi
{
	namespace Utils
	{
		const VkShaderStageFlagBits GetShaderStageFromString(const std::string& type)
		{
			if (type == "vertex") return VK_SHADER_STAGE_VERTEX_BIT;
			if (type == "fragment") return VK_SHADER_STAGE_FRAGMENT_BIT;

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

		const std::filesystem::path GetCacheDirectory()
		{
			return "cache/vulkan/shaders/";
		}
	}

	VulkanShader::VulkanShader(std::string_view path) : filePath(path), name(std::filesystem::path(path).stem().string())
	{	
		auto device = VulkanContext::GetLogicalDevice();

		//auto directory = Utils::GetCacheDirectory();
		//directory += std::filesystem::path(name);
		//directory += Utils::GetCacheExtension(VK_SHADER_STAGE_VERTEX_BIT);
		//if (std::filesystem::directory_entry(directory).exists())
		//{
		//	bool ret = true;
		//	ret = false;
		//}

		std::string shaderCode = Utils::ReadFile(path.data());
		ProcessShader(shaderCode);
	}
	
	VulkanShader::~VulkanShader()
	{
	}
	
	void VulkanShader::Destroy()
	{
		auto device = VulkanContext::GetLogicalDevice();

		for (auto& [stage, module] : shaders)
		{
			vkDestroyShaderModule(device, module, nullptr);
		}

		shaders.clear();
	}

	VkDescriptorSet VulkanShader::AllocateDescriptorSet()
	{
		auto device = VulkanContext::GetLogicalDevice();

		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = VulkanContext::GetDescriptorPool();
		allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		allocInfo.pSetLayouts = descriptorSetLayouts.data();

		VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet));

		return descriptorSet;
	}
	
	void VulkanShader::ProcessShader(std::string code)
	{
		ILG_PROFILE_FUNC

		auto device = VulkanContext::GetLogicalDevice();

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

			std::filesystem::path filename = filePath;
			std::filesystem::path cacheFile = Utils::GetCacheDirectory();
			cacheFile += filename.stem();
			cacheFile += Utils::GetCacheExtension(stage);

			std::ofstream file(cacheFile, std::ios::out | std::ios::binary);

			if (file.is_open())
			{
				file.write((char*)result.data(), result.size() * sizeof(uint32_t));
				file.flush();
				file.close();
			}

			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = 4 * result.size();
			createInfo.pCode = result.data();

			VkShaderModule shaderModule = nullptr;
			VK_CHECK_RESULT(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule));

			ILG_CORE_TRACE("VulkanShader::Reflect - {0} {1}", Utils::ShaderStageToString(stage), filePath);

			ReflectShader(result, stage);

			shaders.push_back({ stage, shaderModule });
		}

		descriptorSetLayouts.resize(descriptorSetBindings.size());
		{
			for (int i = 0; i < descriptorSetLayouts.size(); ++i)
			{
				VkDescriptorSetLayoutCreateInfo layoutInfo{};
				layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layoutInfo.bindingCount = descriptorSetBindings[i].size();
				layoutInfo.pBindings = descriptorSetBindings[i].data();

				VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayouts[i]));
			}
		}

		// TODO: Change this and automatize with reflect function
		//{
		//	VkDescriptorSetLayoutBinding samplerLayoutBinding;
		//	samplerLayoutBinding.binding = 0;
		//	samplerLayoutBinding.descriptorCount = 1;
		//	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		//	samplerLayoutBinding.pImmutableSamplers = nullptr;
		//	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		//
		//	VkDescriptorSetLayoutCreateInfo layoutInfo{};
		//	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		//	layoutInfo.bindingCount = 1;
		//	layoutInfo.pBindings = &samplerLayoutBinding;
		//
		//	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
		//	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout));
		//
		//	descriptorSetLayouts.push_back(descriptorSetLayout);
		//}
	}
	
	const std::vector<uint32_t> VulkanShader::ConvertToSpirV(VkShaderStageFlagBits stage, std::string_view code) const
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(code.data(), Utils::GLShaderStageToShaderC(stage), filePath.c_str(), options);
		
		if (module.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			ILG_CORE_ERROR("{0}", module.GetErrorMessage().c_str());
			ILG_ASSERT(false, "");
		}

		return std::vector<uint32_t>(module.cbegin(), module.cend());
	}
	
	void VulkanShader::ReflectShader(const std::vector<uint32_t>& code, VkShaderStageFlags stage)
	{
		spirv_cross::Compiler compiler(code);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		// Reflecting push constants
		auto constants = resources.push_constant_buffers;
		for (auto& pushConstant : constants)
		{
			const auto& type = compiler.get_type(pushConstant.base_type_id);
			uint32_t size = static_cast<uint32_t>(compiler.get_declared_struct_size(type));
			uint32_t binding = compiler.get_decoration(pushConstant.id, spv::DecorationBinding);
			uint32_t membersCount = static_cast<uint32_t>(type.member_types.size());

			ILG_CORE_TRACE("Push Constant: {0}", compiler.get_name(pushConstant.base_type_id));
			ILG_CORE_TRACE("	Size: {0}", size);
			ILG_CORE_TRACE("	Binding: {0}", binding);
			ILG_CORE_TRACE("	Members: {0}", membersCount);

			VkPushConstantRange pushConstant;
			pushConstant.offset = 0;
			pushConstant.size = size;
			pushConstant.stageFlags = stage;
			
			pushConstants.push_back(pushConstant);
		}

		// Reflecting uniform buffers
		auto resUniformBuffers = resources.uniform_buffers;
		for (auto& uniformBuffer : resUniformBuffers)
		{
			const auto& type = compiler.get_type(uniformBuffer.base_type_id);
			uint32_t size = static_cast<uint32_t>(compiler.get_declared_struct_size(type));
			uint32_t binding = compiler.get_decoration(uniformBuffer.id, spv::DecorationBinding);
			uint32_t set = compiler.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet);
			uint32_t membersCount = static_cast<uint32_t>(type.member_types.size());

			ILG_CORE_TRACE("Uniform Buffer: {0}", uniformBuffer.name.c_str());
			ILG_CORE_TRACE("	Size: {0}", size);
			ILG_CORE_TRACE("	Binding: {0}", binding);
			ILG_CORE_TRACE("	Members: {0}", membersCount);

			VkDescriptorSetLayoutBinding layoutBinding = {};
			layoutBinding.binding = binding;
			layoutBinding.descriptorCount = 1;
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layoutBinding.pImmutableSamplers = nullptr;
			layoutBinding.stageFlags = stage;

			descriptorSetBindings[set].push_back(layoutBinding);
		}

		// Reflecting sampled images
		auto sampledImages = resources.sampled_images;
		for (auto& sampledImage : sampledImages)
		{
			const auto& type = compiler.get_type(sampledImage.base_type_id);
			uint32_t binding = compiler.get_decoration(sampledImage.id, spv::DecorationBinding);
			uint32_t set = compiler.get_decoration(sampledImage.id, spv::DecorationDescriptorSet);

			ILG_CORE_TRACE("Sampler2D: {0}", sampledImage.name.c_str());
			ILG_CORE_TRACE("	Binding: {0}", binding);
			ILG_CORE_TRACE("	Descriptor Set: {0}", set);

			VkDescriptorSetLayoutBinding layoutBinding = {};
			layoutBinding.binding = binding;
			layoutBinding.descriptorCount = 1;
			layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			layoutBinding.pImmutableSamplers = nullptr;
			layoutBinding.stageFlags = stage;

			descriptorSetBindings[set].push_back(layoutBinding);
		}

		// Reflecting separate images
		auto sepImages = resources.separate_images;
		for (auto& separateImage : sepImages)
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
		auto sepSamplers = resources.separate_samplers;
		for (auto& separateSampler : sepSamplers)
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