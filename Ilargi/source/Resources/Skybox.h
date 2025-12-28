#pragma once

#include "Resources/Resource.h"

namespace Ilargi
{
	class Shader;

	class Skybox : public Resource
	{
	public:
		[[nodiscard]] static ResourceType GetStaticType() { return ResourceType::SKYBOX; }
		[[nodiscard]] const ResourceType GetType() const { return GetStaticType(); }

		[[nodiscard]] virtual const void* GetDescriptorSet() const = 0;

		static std::shared_ptr<Skybox> Create(std::shared_ptr<Shader> aShader);
	};
}