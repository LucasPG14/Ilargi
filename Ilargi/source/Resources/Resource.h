#pragma once

#include "Base/UUID.h"

namespace Ilargi
{
	enum class ResourceType
	{
		NONE = 0,
		MODEL,
		MESH,
		TEXTURE2D,
		MATERIAL,
		SCENE,
		SKYBOX
	};

	struct ResourceMetadata
	{
		ResourceType type = ResourceType::NONE;
		std::filesystem::path filepath;
		std::filesystem::path sourceFile;
		std::filesystem::file_time_type lastWriteTime;
	};

	class Resource : public std::enable_shared_from_this<Resource>
	{
	public:
		/*
		* @brief Returns the resource type.
		* @return The resource type.
		*/
		[[nodiscard]] virtual const ResourceType GetType() const = 0;

		/*
		* @brief Casts the resource to the specified template class.
		* @tparam The destination type to which the resource will be cast.
		* @return An instance of type 'T' created from the resource.
		*/
		template <typename T>
		std::shared_ptr<T> As()
		{
			ILG_STATIC_ASSERT(std::is_base_of<Resource, T>::value, "T must be a derived class of Resource");

			return std::static_pointer_cast<T>(shared_from_this());
		}

	public:
		UUID mResourceUUID; // The resource identifier.
	};
}