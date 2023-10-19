#pragma once

namespace Ilargi
{
	class RenderPass;
	class Shader;
	class CommandBuffer;
	class VertexBuffer;
	class IndexBuffer;
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
		Layout() : stride(0) {}

		Layout(const std::initializer_list<Element>& elem) : elements(elem)
		{
			CalculateOffset();
		}

		inline uint32_t GetStride() const { return stride; }
		inline const std::vector<Element>& GetElements() const { return elements; }

		std::vector<Element>::iterator begin() { return elements.begin(); }
		std::vector<Element>::iterator end() { return elements.end(); }

		std::vector<Element>::const_iterator begin() const { return elements.cbegin(); }
		std::vector<Element>::const_iterator end() const { return elements.cend(); }
	
	private:
		void CalculateOffset()
		{
			uint32_t offset = 0;
			stride = 0;
			for (auto& element : elements)
			{
				element.offset = offset;
				offset += element.size;
				stride += element.size;
			}
		}

	private:
		std::vector<Element> elements;
		uint32_t stride;
	};

	struct PipelineProperties
	{
		std::string name;
		Layout layout;
		std::shared_ptr<Shader> shader;
		bool depth = true;
	};

	class Pipeline
	{
	public:
		virtual void Destroy() = 0;

		virtual void PushConstants(std::shared_ptr<CommandBuffer> commandBuffer, uint32_t offset, uint32_t size, const void* data) const = 0;

		virtual void Bind(std::shared_ptr<CommandBuffer> commandBuffer) const = 0;
		virtual void BindDescriptorSet(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<Material> material) const = 0;

		virtual const PipelineProperties& GetProperties() const = 0;

		static std::shared_ptr<Pipeline> Create(const PipelineProperties& props);
	};
}