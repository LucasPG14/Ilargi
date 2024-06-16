#pragma once

namespace Ilargi
{
	struct ComponentNode
	{
	};

	struct TransformNode : public ComponentNode
	{
		vec3 position;
		vec3 rotation;
		vec3 scale;
	};

	struct MeshNode : public ComponentNode
	{
		uint64_t uuid;
	};

	struct EntityNode
	{
		std::vector<ComponentNode> components;
		std::vector<EntityNode> children;
	};

	class StaticMesh;
	class Scene;

	class ModelImporter
	{
	public:
		static std::shared_ptr<StaticMesh> ImportModel(const std::string path);

		static void ImportModel2(const std::filesystem::path& path, const std::filesystem::path& assetsPath);
		
		static void ImportFBX(const std::filesystem::path& path, const std::shared_ptr<Scene>& scene);
	};
}