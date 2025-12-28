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
				if (mMaterial->GetDiffuse())
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
						const auto& metadata{ ResourceManager::GetResourcesMetadata()[uuid] };

						mMaterial->UpdateDiffuse(std::static_pointer_cast<Texture2D>(ResourceManager::GetResource(uuid)));
					}
				}

				if (ImGui::ColorEdit4("Color", glm::value_ptr(mMaterial->GetMaterialData().color)))
				{
					mMaterial->UpdateMaterialData();
				}

				if (ImGui::Button("Save"))
				{
					ResourceManager::SaveResource(mMaterial);
					//MaterialImporter::SaveMaterial(ResourceManager::GetMetadata(mMaterial->mResourceUUID), );
				}

				ImGui::End();
			}
		}
	}
	
	void MaterialPanel::SetMaterial(const std::shared_ptr<Material>& aMaterial)
	{
		mMaterial = aMaterial;
	}
}