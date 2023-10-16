#include "ilargipch.h"

// Main headers
#include "Shader.h"
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanShader.h"

namespace Ilargi
{
	std::shared_ptr<Shader> Shader::Create(std::string_view vert)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanShader>(vert);
		}

		ILG_ASSERT(nullptr, "The platform specified is not supported");
		return nullptr;
	}
	
	ShaderLibrary::ShaderLibrary()
	{
	}
	
	ShaderLibrary::~ShaderLibrary()
	{
	}
	
	void ShaderLibrary::Add(std::string name, std::shared_ptr<Shader> shader)
	{
		ILG_ASSERT(shaders.find(name) == shaders.end(), "This shader already exists!")
		shaders[name] = shader;
	}

	void ShaderLibrary::Add(std::string filepath)
	{
		std::shared_ptr<Shader> shader = Shader::Create(filepath);
		std::string name = std::filesystem::path(filepath).stem().string();

		Add(name, shader);
	}
	
	std::shared_ptr<Shader> ShaderLibrary::Get(std::string name)
	{
		ILG_ASSERT(shaders.find(name) != shaders.end(), "This shader doesn't exists!");
		return shaders[name];
	}
}