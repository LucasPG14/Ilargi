#pragma once

namespace Ilargi
{
	class Material;

	class MaterialPanel
	{
	public:
		/*
		* @brief Constructor.
		*/
		MaterialPanel();

		/*
		* @brief Destructor.
		*/
		~MaterialPanel();

		/*
		* @brief Renders the material panel.
		*/
		void Render();

		/*
		* @brief Sets the material to be modified.
		* @param aMaterial The material to be modified.
		*/
		void SetMaterial(const std::shared_ptr<Material>& aMaterial);

	private:
		std::shared_ptr<Material> mMaterial; // The material to be modified from the material panel.
	};
}