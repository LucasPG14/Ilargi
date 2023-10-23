#pragma once

#include "Resources/Resource.h"

namespace Ilargi
{
	class Shader;

	class Material : public Resource
	{
	public:
		const ResourceType GetType() const { return ResourceType::MATERIAL; }

		virtual const void* GetDescriptorSet() const = 0;

		static std::shared_ptr<Material> Create(std::shared_ptr<Shader> shader);
	};
}