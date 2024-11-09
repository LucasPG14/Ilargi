#pragma once

#include "Resources/Resource.h"

namespace Ilargi
{
	class Shader;
	class Texture2D;

	struct MaterialData
	{
		glm::vec4 color = glm::vec4(1.0f);
		float metallic = 0.5f;
		float roughness = 0.5f;
	};

	class Material : public Resource
	{
	public:
		static ResourceType GetStaticType() { return ResourceType::MATERIAL; }
		const ResourceType GetType() const { return GetStaticType(); }

		virtual const void* GetDescriptorSet() const = 0;
		virtual const MaterialData& GetMaterialData() const = 0;
		virtual MaterialData& GetMaterialData() = 0;

		virtual std::shared_ptr<Texture2D> GetDiffuse() = 0;
		virtual void UpdateDiffuse(std::shared_ptr<Texture2D> aTexture) = 0;

		virtual void UpdateMaterialData() = 0;

		static std::shared_ptr<Material> Create(std::shared_ptr<Shader> aShader);
	};
}