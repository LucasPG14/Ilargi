#pragma once

#include "DescriptorSetLayout.h"

namespace Ilargi
{
	struct PushConstantRange
	{
		bool operator==(const PushConstantRange& aPushConstant) const
		{
			return Size == aPushConstant.Size && Offset == aPushConstant.Offset && Stage == aPushConstant.Stage;
		}

		uint32_t Size{0U};
		uint32_t Offset {0U};
		ShaderStage Stage;
	};

	struct PipelineLayoutProperties
	{
		bool operator==(const PipelineLayoutProperties& aProperties) const
		{
			return DescriptorSetLayoutsProperties == aProperties.DescriptorSetLayoutsProperties && 
				PushConstantRanges == aProperties.PushConstantRanges;
		}

		std::vector<DescriptorSetLayoutProperties> DescriptorSetLayoutsProperties;
		std::vector<PushConstantRange> PushConstantRanges;
	};

	class PipelineLayout : public std::enable_shared_from_this<PipelineLayout>
	{
	public:
		/*
		* @brief Casts the pipeline layout to the specified template class.
		* @tparam The destination type to which the pipeline layout will be cast.
		* @return An instance of type 'T' created from the pipeline layout.
		*/
		template <typename T>
		std::shared_ptr<T> As()
		{
			ILG_STATIC_ASSERT(std::is_base_of<PipelineLayout, T>::value, "T must be a derived class of PipelineLayout");

			return std::static_pointer_cast<T>(shared_from_this());
		}

		/*
		* @brief Creates the pipeline layout.
		* @param aProperties The properties of the pipeline layout.
		* @return An instance of the pipeline layout created.
		*/
		static std::shared_ptr<PipelineLayout> Create(const PipelineLayoutProperties& aProperties);
	};
}

namespace std
{
	template<>
	struct hash<Ilargi::PushConstantRange>
	{
		size_t operator()(const Ilargi::PushConstantRange& aPushConstant) const
		{
			size_t h{ hash<uint32_t>{}(aPushConstant.Size) };
			h ^= hash<uint32_t>{}(aPushConstant.Offset) << 1;
			h ^= hash<int>{}(static_cast<int>(aPushConstant.Stage)) << 2;

			return h;
		}
	};

	template<>
	struct hash<Ilargi::PipelineLayoutProperties>
	{
		size_t operator()(const Ilargi::PipelineLayoutProperties& aProperties) const
		{
			size_t h{};
			for (const auto& DescriptorBinding : aProperties.DescriptorSetLayoutsProperties)
				h ^= hash<Ilargi::DescriptorSetLayoutProperties>{}(DescriptorBinding);
			for (const auto& PushConstant : aProperties.PushConstantRanges)
				h ^= hash<Ilargi::PushConstantRange>{}(PushConstant) << 1;

			return h;
		}
	};
}