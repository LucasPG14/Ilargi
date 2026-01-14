#pragma once

namespace Ilargi
{
	enum class DescriptorType
	{
		UNIFORM_BUFFER = 0,
		COMBINED_IMAGE_SAMPLER = 1
	};

	enum ShaderStage
	{
		VERTEX_SHADER = 0x00000001,
		FRAGMENT_SHADER = 0x00000002,
	};

	struct BindingInfo
	{
		uint32_t set;
		uint32_t binding;
		DescriptorType type;
	};

	class Shader : public std::enable_shared_from_this<Shader>
	{
	public:
		/*
		* @brief Destroys the shader data.
		*/
		virtual void Destroy() = 0;

		/*
		* @brief Returns the name of the shader.
		* @return The name of the shader.
		*/
		virtual const std::string& GetName() const = 0;

		/*
		* @brief Casts the shader to the specified template class.
		* @tparam The destination type to which the shader will be cast.
		* @return An instance of type 'T' created from the shader.
		*/
		template <typename T>
		std::shared_ptr<T> As()
		{
			ILG_STATIC_ASSERT(std::is_base_of<Shader, T>::value, "T must be a derived class of Shader");

			return std::static_pointer_cast<T>(shared_from_this());
		}

		/*
		* @brief Creates the shader.
		* @param aFilepath The filepath of the shader.
		* @return An instance of the shader created.
		*/
		static std::shared_ptr<Shader> Create(std::string_view aFilepath);
	};

	class ShaderLibrary
	{
	public:
		ShaderLibrary();
		~ShaderLibrary();

		/*
		* @brief Creates all the shaders.
		*/
		void Init();

		/*
		* @brief Adds a shader given a name and the shader instance.
		* @param aName The shader name.
		* @param aShader An instance of the shader.
		*/
		void Add(std::string aName, const std::shared_ptr<Shader>& aShader);

		/*
		* @brief Adds a shader given a filepath.
		* @param aFilepath The shader filepath.
		*/
		void Add(std::string aFilepath);

		/*
		* @brief Returns a shader by its name.
		* @param aName The shader name.
		* @return An instance of the shader.
		*/
		[[nodiscard]] const std::shared_ptr<Shader>& Get(std::string aName);

	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> mShaders; // The container of the shaders.
	};
}