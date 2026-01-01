#include "ilargipch.h"

// Main headers
#include "Shader.h"
#include "Renderer.h"

// Platform headers
#include "Platform/Vulkan/VulkanShader.h"

namespace Ilargi
{
	std::shared_ptr<Shader> Shader::Create(std::string_view aCode)
	{
		switch (Renderer::GetGraphicsAPI())
		{
		case GraphicsAPI::VULKAN:	return std::make_shared<VulkanShader>(aCode);
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

	void ShaderLibrary::Init()
	{
		Add("Shaders/PBR_Static.shader");
		Add("Shaders/Grid.shader");
		Add("Shaders/Outline.shader");
	}
	
	void ShaderLibrary::Add(std::string aName, const std::shared_ptr<Shader>& aShader)
	{
		ILG_ASSERT(mShaders.find(aName) == mShaders.end(), "This shader already exists!")
		mShaders[aName] = aShader;
	}

	void ShaderLibrary::Add(std::string aFilepath)
	{
		std::shared_ptr<Shader> shader{ Shader::Create(aFilepath) };
		std::string name{ std::filesystem::path(aFilepath).stem().string() };

		Add(name, shader);
	}
	
	const std::shared_ptr<Shader>& ShaderLibrary::Get(std::string aName)
	{
		ILG_ASSERT(mShaders.find(aName) != mShaders.end(), "This shader doesn't exists!");
		return mShaders[aName];
	}
}