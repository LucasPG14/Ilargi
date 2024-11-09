#pragma once

namespace Ilargi
{
	class Shader
	{
	public:
		virtual void Destroy() = 0;

		static std::shared_ptr<Shader> Create(std::string_view aCode);
	};

	class ShaderLibrary
	{
	public:
		ShaderLibrary();
		~ShaderLibrary();

		void Init();

		void Add(std::string aName, std::shared_ptr<Shader> aShader);
		void Add(std::string aFilepath);

		std::shared_ptr<Shader> Get(std::string aName);

	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> mShaders;
	};
}