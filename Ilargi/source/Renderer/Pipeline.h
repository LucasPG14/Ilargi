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

		inline uint32_t GetStride() const { return mStride; }
		inline const std::vector<Element>& GetElements() const { return mElements; }

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
		bool depth = true;
		std::shared_ptr<Shader> shader;
		Layout layout;
	};

	class Pipeline
	{
	public:
		virtual void Destroy() = 0;

		virtual void PushConstants(const std::shared_ptr<CommandBuffer>& aCommandBuffer, uint32_t aOffset, uint32_t aSize, const void* aData) const = 0;

		virtual void Bind(const std::shared_ptr<CommandBuffer>& aCommandBuffer) const = 0;
		virtual void BindDescriptorSet(const std::shared_ptr<CommandBuffer>& aCommandBuffer, std::shared_ptr<Material> aMaterial, uint32_t aSetIndex) const = 0;
		virtual void BindDescriptorSet(const std::shared_ptr<CommandBuffer>& aCommandBuffer, std::shared_ptr<UniformBuffer> aUniformBuffer, uint32_t aSetIndex) const = 0;

		virtual const PipelineProperties& GetProperties() const = 0;

		static std::shared_ptr<Pipeline> Create(const PipelineProperties& aProperties);
	};
}