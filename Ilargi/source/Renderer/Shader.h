#pragma once

namespace Ilargi
{
	class Shader : public std::enable_shared_from_this<Shader>
	{
	public:
		virtual void Destroy() = 0;
		virtual const std::string& GetName() const = 0;

		template <typename T>
		std::shared_ptr<T> As()
		{
			ILG_STATIC_ASSERT(std::is_base_of<Shader, T>::value, "T must be a derived class of Shader");

			return std::static_pointer_cast<T>(shared_from_this());
		}

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