#include "ilargipch.h"

#include "MaterialPanel.h"

#include "Resources/Material.h"
#include "Resources/ResourceManager.h"
#include "Resources/Texture.h"
#include "Utils/Importers/MaterialImporter.h"

#include <imgui/imgui.h>
#include <gtc/type_ptr.hpp>

namespace Ilargi
{
	MaterialPanel::MaterialPanel() : mMaterial(nullptr)
	{

	}
	
	MaterialPanel::~MaterialPanel()
	{

	}
	
	void MaterialPanel::Render()
	{
		if (mMaterial)
		{
			if (ImGui::Begin("Material Editor"))
			{
				const auto& bindings{ mMaterial->GetBindings() };
				for (const auto& [name, binding] : bindings)
				{
					const std::shared_ptr<Texture2D>& texture{ mMaterial->GetTexture(name) };
					if (texture)
					{
						ImGui::Image((ImTextureID)texture->GetID(), {32, 32});
					}
					ImGui::SameLine();
					ImGui::Text(name.c_str());
				}

				if (ImGui::ColorEdit4("Color", glm::value_ptr(mMaterial->GetMaterialData().color)))
				{
					mMaterial->UpdateMaterialData();
				}

				if (ImGui::Button("Save"))
				{
					ResourceManager::SaveResource(mMaterial);
				}
				/*if (mMaterial->GetDiffuse())
				{
					ImGui::Image((void*)mMaterial->GetDiffuse()->GetID(), { 32, 32 });
				}
				else
				{
					ImGui::Text("Diffuse");
				}
				if (ImGui::BeginDragDropTarget())
				{
					auto payload{ ImGui::AcceptDragDropPayload("TEXTURE2D") };

					if (payload)
					{
						UUID uuid{ *(UUID*)payload->Data };
						const auto& metadata{ ResourceManager::GetMetadata(uuid) };

						mMaterial->UpdateDiffuse(std::static_pointer_cast<Texture2D>(ResourceManager::GetResource(uuid)));
					}
				}*/
			}
			ImGui::End();
		}
	}
	
	void MaterialPanel::SetMaterial(const std::shared_ptr<Material>& aMaterial)
	{
		mMaterial = aMaterial;
	}
}