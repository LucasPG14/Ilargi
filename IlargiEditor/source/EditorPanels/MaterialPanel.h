#pragma once

namespace Ilargi
{
	class Material;

	class MaterialPanel
	{
	public:
		MaterialPanel();
		~MaterialPanel();

		void Render();

		void SetMaterial(const std::shared_ptr<Material>& aMaterial);

	private:
		std::shared_ptr<Material> mMaterial;
	};
}