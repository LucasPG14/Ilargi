#pragma once

#include "Resources/Resource.h"

namespace Ilargi
{
	class Shader;
	class Texture2D;

	struct MaterialData
	{
		glm::vec4 color{ glm::vec4(1.0f) };
		float metallic{ 0.5f };
		float roughness{ 0.5f };
	};

	class Material : public Resource
	{
	public:
		[[nodiscard]] static ResourceType GetStaticType() { return ResourceType::MATERIAL; }
		[[nodiscard]] const ResourceType GetType() const { return GetStaticType(); }

		[[nodiscard]] virtual const std::shared_ptr<Shader>& GetShader() const = 0;
		
		[[nodiscard]] virtual const void* GetDescriptorSet() const = 0;
		[[nodiscard]] virtual const MaterialData& GetMaterialData() const = 0;
		[[nodiscard]] virtual MaterialData& GetMaterialData() = 0;

		[[nodiscard]] virtual std::shared_ptr<Texture2D> GetDiffuse() = 0;
		virtual void UpdateDiffuse(std::shared_ptr<Texture2D> aTexture) = 0;

		virtual void UpdateMaterialData() = 0;

		static std::shared_ptr<Material> Create(std::shared_ptr<Shader> aShader, const MaterialData& aMaterialData = {});
	};
}