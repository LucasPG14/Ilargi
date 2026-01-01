#pragma once

namespace Ilargi
{
	class RenderPass;
	class Shader;
	class CommandBuffer;
	class VertexBuffer;
	class IndexBuffer;
	class UniformBuffer;
	class Material;

	enum class ShaderDataType
	{
		NONE = 0,
		FLOAT,
		FLOAT2,
		FLOAT3,
		FLOAT4,
		INT,
		INT2,
		INT3,
		INT4
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::FLOAT:		return 4;
		case ShaderDataType::FLOAT2:	return 8;
		case ShaderDataType::FLOAT3:	return 12;
		case ShaderDataType::FLOAT4:	return 16;
		case ShaderDataType::INT:		return 4;
		case ShaderDataType::INT2:		return 8;
		case ShaderDataType::INT3:		return 12;
		case ShaderDataType::INT4:		return 16;
		}

		ILG_ASSERT(nullptr, "Shader data type not supported");
		return 0;
	}

	struct Element
	{
		ShaderDataType type;
		std::string name;
		uint32_t size;
		uint32_t offset;

		Element() : name(""), type(ShaderDataType::NONE), size(0), offset(0) {}

		Element(const ShaderDataType elemType, const std::string& elemName, bool normalized = false)
			: type(elemType), name(elemName), size(ShaderDataTypeSize(elemType)), offset(0) {}
	};

	class Layout
	{
	public:
		Layout() : mStride(0) {}

		Layout(const std::initializer_list<Element>& aElements) : mElements(aElements)
		{
			CalculateOffset();
		}

		[[nodiscard]] inline uint32_t GetStride() const { return mStride; }
		[[nodiscard]] inline const std::vector<Element>& GetElements() const { return mElements; }

		constexpr std::vector<Element>::iterator begin() { return mElements.begin(); }
		constexpr std::vector<Element>::iterator end() { return mElements.end(); }

		constexpr std::vector<Element>::const_iterator begin() const { return mElements.cbegin(); }
		constexpr std::vector<Element>::const_iterator end() const { return mElements.cend(); }
	
	private:
		void CalculateOffset()
		{
			uint32_t offset = 0;
			mStride = 0;
			for (auto& element : mElements)
			{
				element.offset = offset;
				offset += element.size;
				mStride += element.size;
			}
		}

	private:
		std::vector<Element> mElements;
		uint32_t mStride;
	};

	struct PipelineProperties
	{
		std::string name;
		std::shared_ptr<RenderPass> renderPass;
		std::shared_ptr<Shader> shader;
		Layout layout;
		bool testDepth = true;
		bool writeDepth = true;
		bool hasStencil = true;
		bool writeStencil = true;
	};

	class Pipeline : public std::enable_shared_from_this<Pipeline>
	{
	public:
		/*
		* @brief Destroys the pipeline data.
		*/
		virtual void Destroy() = 0;

		/*
		* @brief Push a constant value to the shader.
		* @param aCommandBuffer The command buffer that will execute the push action.
		* @param aOffset The offset of the data.
		* @param aSize The size of the data.
		* @param aData A pointer to the data pushed to the shader.
		*/
		virtual void PushConstants(const std::shared_ptr<CommandBuffer>& aCommandBuffer, uint32_t aOffset, uint32_t aSize, const void* aData) const = 0;

		/*
		* @brief Binds the pipeline.
		* @param aCommandBuffer The command buffer that will execute the bind of the pipeline.
		*/
		virtual void Bind(const std::shared_ptr<CommandBuffer>& aCommandBuffer) const = 0;

		/*
		* @brief Binds the material.
		* @param aCommandBuffer The command buffer that will execute the bind of the pipeline.
		* @param aMaterial The material to bind.
		* @param aSetIndex The index set to bind the material.
		*/
		virtual void BindMaterial(const std::shared_ptr<CommandBuffer>& aCommandBuffer, const std::shared_ptr<Material>& aMaterial, uint32_t aSetIndex) const = 0;
		
		/*
		* @brief Binds the uniform buffer.
		* @param aCommandBuffer The command buffer that will execute the bind of the uniform buffer.
		* @param aUniformBuffer The uniform buffer to bind.
		* @param aSetIndex The index set to bind the uniform buffer.
		*/
		virtual void BindUniformBuffer(const std::shared_ptr<CommandBuffer>& aCommandBuffer, const std::shared_ptr<UniformBuffer>& aUniformBuffer, uint32_t aSetIndex) const = 0;

		/*
		* @brief Returns the properties of the pipeline.
		* @return The properties of the pipeline.
		*/
		[[nodiscard]] virtual const PipelineProperties& GetProperties() const = 0;

		/*
		* @brief Casts the pipeline to the specified template class.
		* @tparam The destination type to which the pipeline will be cast.
		* @return An instance of type 'T' created from the pipeline.
		*/
		template <typename T>
		std::shared_ptr<T> As()
		{
			ILG_STATIC_ASSERT(std::is_base_of<Pipeline, T>::value, "T must be a derived class of Pipeline");

			return std::static_pointer_cast<T>(shared_from_this());
		}

		/*
		* @brief Creates the pipeline.
		* @param aProperties The properties of the pipeline that will be created.
		* @return An instance of the pipeline created.
		*/
		static std::shared_ptr<Pipeline> Create(const PipelineProperties& aProperties);
	};
}