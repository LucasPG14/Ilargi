#pragma once

namespace Ilargi
{
	class Shader
	{
	public:

		static std::shared_ptr<Shader> Create(std::string_view vert);
	};

	class ShaderLibrary
	{
	public:
		ShaderLibrary();
		~ShaderLibrary();

		void Add(std::string name, std::shared_ptr<Shader> shader);
		void Add(std::string filepath);

		std::shared_ptr<Shader> Get(std::string name);

	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
	};
}