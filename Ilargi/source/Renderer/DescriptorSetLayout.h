#pragma once

namespace Ilargi
{
	enum ShaderStage
	{
		VERTEX_SHADER = 0x01,
		FRAGMENT_SHADER = 0x02
	};

	enum class DescriptorType : uint8_t
	{
		UNIFORM_BUFFER = 0,
		COMBINED_IMAGE_SAMPLER = 1
	};

	struct DescriptorBinding
	{
		bool operator==(const DescriptorBinding& aProperties) const = default;

		uint8_t Binding;
		DescriptorType Type;
		ShaderStage Stage;
	};

	struct DescriptorSetLayoutProperties
	{
		DescriptorSetLayoutProperties(uint8_t aSetNumber, const DescriptorBinding& aDescriptorBindings) :
			SetNumber(aSetNumber)
		{
			DescriptorBindings.push_back(aDescriptorBindings);
		}

		bool operator==(const DescriptorSetLayoutProperties& aProperties) const = default;

		uint8_t SetNumber;
		std::vector<DescriptorBinding> DescriptorBindings;
	};

	class DescriptorSetLayout : public std::enable_shared_from_this<DescriptorSetLayout>
	{
	public:
		/*
		* @brief Casts the descriptor set layout to the specified template class.
		* @tparam The destination type to which the descriptor set layout will be cast.
		* @return An instance of type 'T' created from the descriptor set layout.
		*/
		template <typename T>
		std::shared_ptr<T> As()
		{
			ILG_STATIC_ASSERT(std::is_base_of<DescriptorSetLayout, T>::value, "T must be a derived class of DescriptorSetLayout");

			return std::static_pointer_cast<T>(shared_from_this());
		}

		/*
		* @brief Creates the descriptor set layout.
		* @param aProperties The properties of the descriptor set layout.
		* @return An instance of the descriptor set layout created.
		*/
		static std::shared_ptr<DescriptorSetLayout> Create(const DescriptorSetLayoutProperties& aProperties);
	};
}

namespace std
{
	template<>
	struct hash<Ilargi::DescriptorBinding>
	{
		size_t operator()(const Ilargi::DescriptorBinding& aBinding) const
		{
			size_t h{ std::hash<uint8_t>{}(aBinding.Binding) };
			h ^= std::hash<int>{}(static_cast<int>(aBinding.Type)) << 1;
			h ^= std::hash<int>{}(static_cast<int>(aBinding.Stage)) << 2;
			return h;
		}
	};

	template<>
	struct hash<Ilargi::DescriptorSetLayoutProperties>
	{
		size_t operator()(const Ilargi::DescriptorSetLayoutProperties& aProperties) const
		{
			size_t h{ std::hash<uint8_t>{}(aProperties.SetNumber) };
			for (const auto& DescriptorBinding : aProperties.DescriptorBindings)
				h ^= std::hash<Ilargi::DescriptorBinding>{}(DescriptorBinding) << 1;

			return h;
		}
	};
}