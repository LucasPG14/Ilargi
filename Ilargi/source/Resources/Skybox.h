#pragma once

#include "Resources/Resource.h"

namespace Ilargi
{
	class Shader;

	class Skybox : public Resource
	{
	public:
		static ResourceType GetStaticType() { return ResourceType::SKYBOX; }
		const ResourceType GetType() const { return GetStaticType(); }

		virtual const void* GetDescriptorSet() const = 0;

		static std::shared_ptr<Skybox> Create(std::shared_ptr<Shader> aShader);
	};
}