#pragma once

#include "IFramebuffer.h"

namespace Ilargi
{
	enum ShaderStage;
	class RenderPass;
	class IShader;
	class ICommandBuffer;
	class IVertexBuffer;
	class IIndexBuffer;
	class UniformBuffer;
	class Material;

	enum class ShaderDataType
	{
		NONE = 0,
		FLOAT_16,
		FLOAT2_16,
		FLOAT3_16,
		FLOAT4_16,
		FLOAT_32,
		FLOAT2_32,
		FLOAT3_32,
		FLOAT4_32,
		INT_16,
		INT2_16,
		INT3_16,
		INT4_16,
		INT_32,
		INT2_32,
		INT3_32,
		INT4_32
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::FLOAT_16:		return 2;
		case ShaderDataType::FLOAT2_16:		return 4;
		case ShaderDataType::FLOAT3_16:		return 6;
		case ShaderDataType::FLOAT4_16:		return 8;

		case ShaderDataType::FLOAT_32:		return 4;
		case ShaderDataType::FLOAT2_32:		return 8;
		case ShaderDataType::FLOAT3_32:		return 12;
		case ShaderDataType::FLOAT4_32:		return 16;

		case ShaderDataType::INT_16:		return 2;
		case ShaderDataType::INT2_16:		return 4;
		case ShaderDataType::INT3_16:		return 6;
		case ShaderDataType::INT4_16:		return 8;

		case ShaderDataType::INT_32:		return 4;
		case ShaderDataType::INT2_32:		return 8;
		case ShaderDataType::INT3_32:		return 12;
		case ShaderDataType::INT4_32:		return 16;
		}

		ILG_ASSERT(nullptr, "Shader data type not supported");
		return 0;
	}

	struct Element
	{
		ShaderDataType Type;
		std::string Name;
		uint32_t Size;
		uint32_t Offset;

		Element() : Name(""), Type(ShaderDataType::NONE), Size(0U), Offset(0U) {}

		Element(const ShaderDataType aElemType, const std::string& aElemName, bool aNormalized = false)
			: Type(aElemType), Name(aElemName), Size(ShaderDataTypeSize(aElemType)), Offset(0U) {}

		bool operator==(const Element&) const = default;
	};

	struct VertexLayout
	{
	public:
		VertexLayout() : mStride(0) {}

		VertexLayout(const std::initializer_list<Element>& aElements) : mElements(aElements)
		{
			CalculateOffset();
		}

		[[nodiscard]] inline uint32_t GetStride() const { return mStride; }
		[[nodiscard]] inline const std::vector<Element>& GetElements() const { return mElements; }

		constexpr std::vector<Element>::iterator begin() { return mElements.begin(); }
		constexpr std::vector<Element>::iterator end() { return mElements.end(); }

		constexpr std::vector<Element>::const_iterator begin() const { return mElements.cbegin(); }
		constexpr std::vector<Element>::const_iterator end() const { return mElements.cend(); }
		
		bool operator==(const VertexLayout&) const = default;
	
	private:
		void CalculateOffset()
		{
			uint32_t offset = 0;
			mStride = 0;
			for (auto& element : mElements)
			{
				element.Offset = offset;
				offset += element.Size;
				mStride += element.Size;
			}
		}

	public:
		std::vector<Element> mElements;
		uint32_t mStride;
	};

	enum class CullMode : uint8_t
	{
		NONE,
		FRONT,
		BACK
	};

	enum class FillMode : uint8_t
	{
		FILL,
		LINE, 
		POINT
	};

	enum class FrontFace : uint8_t
	{
		COUNTER_CLOCKWISE,
		CLOCKWISE
	};

	enum class StencilOp : uint8_t
	{
		KEEP,
		ZERO,
		REPLACE,
		INCREMENT_WRAP,
		INCREMENT_CLAMP,
		DECREMENT_WRAP,
		DECREMENT_CLAMP,
		INVERT
	};

	enum class BlendOp : uint8_t
	{
		ADD,
		SUBSTRACT,
		REVERSE_SUBSTRACT,
		MIN,
		MAX
	};

	enum class CompareOp : uint8_t
	{
		NEVER,
		LESS,
		EQUAL,
		LESS_EQUAL,
		GREATER,
		NOT_EQUAL,
		GREATER_EQUAL,
		ALWAYS
	};

	enum class BlendFactor : uint8_t
	{
		ZERO,
		ONE,
		SRC_COLOR,
		ONE_MINUS_SRC_COLOR,
		DST_COLOR,
		ONE_MINUS_DST_COLOR,
		SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA,
		DST_ALPHA,
		ONE_MINUS_DST_ALPHA,
		CONSTANT_COLOR,
		ONE_MINUS_CONSTANT_COLOR
	};

	enum class ColorMask : uint8_t
	{
		NONE,
		R,
		RG,
		RGB,
		RGBA
	};

	struct StencilFaceState
	{
		CompareOp CompareOp{ CompareOp::ALWAYS };
		StencilOp PassOp{ StencilOp::KEEP };
		StencilOp FailOp{ StencilOp::KEEP };
		StencilOp DepthFailOp{ StencilOp::KEEP };
		uint8_t CompareMask{ 0xFFU };
		uint8_t WriteMask{ 0xFFU };
		uint8_t Reference{ 1U };

		bool operator==(const StencilFaceState&) const = default;
	};

	struct BlendState
	{
		bool Enabled{true};
		BlendFactor SrcColor{ BlendFactor::SRC_ALPHA };
		BlendFactor DstColor{ BlendFactor::ONE_MINUS_SRC_ALPHA };
		BlendOp ColorOp{ BlendOp::ADD };
		BlendFactor SrcAlpha{ BlendFactor::ONE };
		BlendFactor DstAlpha{ BlendFactor::ZERO };
		BlendOp AlphaOp{ BlendOp::ADD };
		ColorMask ColorMask{ ColorMask::RGBA };

		bool operator==(const BlendState&) const = default;
	};

	struct StencilState
	{
		bool Enabled {false};
		StencilFaceState Front{};
		StencilFaceState Back{};

		bool operator==(const StencilState&) const = default;
	};

	struct DepthState
	{
		bool Enabled {true};
		bool Test {true};
		bool Write {true};
		CompareOp CompareOp{ CompareOp::LESS };
		StencilState StencilState {};

		bool operator==(const DepthState&) const = default;
	};

	struct RasterState
	{
		CullMode Cull{ CullMode::NONE };
		FillMode Fill{ FillMode::FILL };
		FrontFace FrontFace{ FrontFace::COUNTER_CLOCKWISE };
		bool DepthClamp {false};
		bool DepthBias {false};

		bool operator==(const RasterState&) const = default;
	};

	struct GraphicsPipelineProperties
	{
		std::string ShaderName;
		VertexLayout VertexLayout;
		std::vector<ImageFormat> ColorFormats;
		RasterState RasterState;
		DepthState DepthState;
		BlendState BlendState;

		uint32_t SampleCount;

		bool operator==(const GraphicsPipelineProperties& aProperties) const
		{
			return ShaderName == aProperties.ShaderName &&
				VertexLayout == aProperties.VertexLayout &&
				ColorFormats == aProperties.ColorFormats &&
				RasterState == aProperties.RasterState &&
				DepthState == aProperties.DepthState &&
				BlendState == aProperties.BlendState &&
				SampleCount == aProperties.SampleCount;
		};
	};

	class IGraphicsPipeline : public std::enable_shared_from_this<IGraphicsPipeline>
	{
	public:
		/*
		* @brief Destroys the pipeline data.
		*/
		virtual void Destroy() = 0;

		/*
		* @brief Returns the properties of the pipeline.
		* @return The properties of the pipeline.
		*/
		[[nodiscard]] virtual const GraphicsPipelineProperties& GetProperties() const = 0;

		/*
		* @brief Casts the pipeline to the specified template class.
		* @tparam The destination type to which the pipeline will be cast.
		* @return An instance of type 'T' created from the pipeline.
		*/
		template <typename T>
		std::shared_ptr<T> As()
		{
			ILG_STATIC_ASSERT(std::is_base_of<IGraphicsPipeline, T>::value, "T must be a derived class of Pipeline");

			return std::static_pointer_cast<T>(shared_from_this());
		}

		/*
		* @brief Creates the pipeline.
		* @param aProperties The properties of the pipeline that will be created.
		* @return An instance of the pipeline created.
		*/
		static std::shared_ptr<IGraphicsPipeline> Create(const GraphicsPipelineProperties& aProperties);
	};
}

namespace std
{
	template<>
	struct hash<Ilargi::RasterState>
	{
		size_t operator()(const Ilargi::RasterState& aRasterState) const
		{
			size_t h{ std::hash<int>{}(static_cast<int>(aRasterState.Cull)) };
			h ^= std::hash<int>{}(static_cast<int>(aRasterState.Fill)) << 1;
			h ^= std::hash<int>{}(static_cast<int>(aRasterState.FrontFace)) << 2;
			h ^= std::hash<bool>{}(aRasterState.DepthClamp) << 3;
			h ^= std::hash<bool>{}(aRasterState.DepthBias) << 4;
			return h;
		}
	};

	template<>
	struct hash<Ilargi::BlendState>
	{
		size_t operator()(const Ilargi::BlendState& aBlendState) const
		{
			size_t h{ std::hash<bool>{}(aBlendState.Enabled) };
			h ^= std::hash<int>{}(static_cast<int>(aBlendState.SrcColor)) << 1;
			h ^= std::hash<int>{}(static_cast<int>(aBlendState.DstColor)) << 2;
			h ^= std::hash<int>{}(static_cast<int>(aBlendState.ColorOp)) << 3;
			h ^= std::hash<int>{}(static_cast<int>(aBlendState.SrcAlpha)) << 4;
			h ^= std::hash<int>{}(static_cast<int>(aBlendState.DstAlpha)) << 5;
			h ^= std::hash<int>{}(static_cast<int>(aBlendState.AlphaOp)) << 6;
			h ^= std::hash<int>{}(static_cast<int>(aBlendState.ColorMask)) << 7;
			return h;
		}
	};

	template<>
	struct hash<Ilargi::StencilFaceState>
	{
		size_t operator()(const Ilargi::StencilFaceState& aStencilFaceState) const
		{
			size_t h{ std::hash<int>{}(static_cast<int>(aStencilFaceState.CompareOp)) };
			h ^= std::hash<int>{}(static_cast<int>(aStencilFaceState.PassOp)) << 1;
			h ^= std::hash<int>{}(static_cast<int>(aStencilFaceState.FailOp)) << 2;
			h ^= std::hash<int>{}(static_cast<int>(aStencilFaceState.DepthFailOp)) << 3;
			h ^= std::hash<uint8_t>{}(aStencilFaceState.CompareMask) << 4;
			h ^= std::hash<uint8_t>{}(aStencilFaceState.WriteMask) << 5;
			h ^= std::hash<uint8_t>{}(aStencilFaceState.Reference) << 6;

			return h;
		}
	};

	template<>
	struct hash<Ilargi::StencilState>
	{
		size_t operator()(const Ilargi::StencilState& aStencilState) const
		{
			size_t h{ std::hash<bool>{}(aStencilState.Enabled) };
			h ^= std::hash<Ilargi::StencilFaceState>{}(aStencilState.Front) << 1;
			h ^= std::hash<Ilargi::StencilFaceState>{}(aStencilState.Back) << 2;

			return h;
		}
	};

	template<>
	struct hash<Ilargi::DepthState>
	{
		size_t operator()(const Ilargi::DepthState& aDepthState) const
		{
			size_t h{ std::hash<bool>{}(aDepthState.Enabled) };
			h ^= std::hash<bool>{}(aDepthState.Test) << 1;
			h ^= std::hash<bool>{}(aDepthState.Write) << 2;
			h ^= std::hash<int>{}(static_cast<int>(aDepthState.CompareOp)) << 3;
			h ^= std::hash<Ilargi::StencilState>{}(aDepthState.StencilState) << 4;
		

			return h;
		}
	};

	template<>
	struct hash<Ilargi::Element>
	{
		size_t operator()(const Ilargi::Element& aElement) const
		{
			size_t h{ std::hash<int>{}(static_cast<int>(aElement.Type)) };
			h ^= std::hash<std::string>{}(aElement.Name) << 1;
			h ^= std::hash<uint32_t>{}(aElement.Size) << 2;
			h ^= std::hash<uint32_t>{}(aElement.Offset) << 3;

			return h;
		}
	};

	template<>
	struct hash<Ilargi::VertexLayout>
	{
		size_t operator()(const Ilargi::VertexLayout& aVertexLayout) const
		{
			size_t h{ std::hash<uint32_t>{}(aVertexLayout.mStride) };
			for (const auto& element : aVertexLayout)
				h ^= std::hash<Ilargi::Element>{}(element) << 2;

			return h;
		}
	};

	template<>
	struct hash<Ilargi::GraphicsPipelineProperties>
	{
		size_t operator()(const Ilargi::GraphicsPipelineProperties& aProperties) const
		{
			size_t h{ std::hash<std::string>{}(aProperties.ShaderName) };
			h ^= std::hash<Ilargi::VertexLayout>{}(aProperties.VertexLayout) << 1;
			for (const auto& format : aProperties.ColorFormats)
				h ^= std::hash<int>{}(static_cast<int>(format)) << 2;

			h ^= std::hash<Ilargi::RasterState>{}(aProperties.RasterState) << 3;
			h ^= std::hash<Ilargi::DepthState>{}(aProperties.DepthState) << 4;
			h ^= std::hash<Ilargi::BlendState>{}(aProperties.BlendState) << 5;
			h ^= std::hash<uint32_t>{}(aProperties.SampleCount) << 6;

			return h;
		}
	};
}