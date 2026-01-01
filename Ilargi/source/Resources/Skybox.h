#pragma once

#include "Resources/Resource.h"

namespace Ilargi
{
	class Shader;

	class Skybox : public Resource
	{
	public:
		/*
		* @brief Returns the resource type.
		* @return The resource type.
		*/
		[[nodiscard]] static ResourceType GetStaticType() { return ResourceType::SKYBOX; }

		/*
		* @copydoc Resource::GetType()
		*/
		[[nodiscard]] const ResourceType GetType() const { return GetStaticType(); }

		/*
		* @brief Returns the descriptor set of the skybox.
		* @return The skybox descriptor set.
		*/
		[[nodiscard]] virtual const void* GetDescriptorSet() const = 0;

		/*
		* @brief Creates the skybox.
		* @param aShader Instance of the shader used by the skybox.
		* @return An instance of the skybox created.
		*/
		static std::shared_ptr<Skybox> Create(const std::shared_ptr<Shader>& aShader);
	};
}