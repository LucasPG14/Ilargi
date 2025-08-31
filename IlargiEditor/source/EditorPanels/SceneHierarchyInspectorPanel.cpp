#include "ilargipch.h"

#include "SceneHierarchyInspectorPanel.h"
#include "Utils/UI/IlargiUI.h"
#include "Base/Input.h"

#include "Resources/ResourceManager.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Resources/Texture.h"

#include <imgui/imgui.h>
#include <gtc/type_ptr.hpp>

namespace Ilargi
{
	SceneHierarchyInspectorPanel::SceneHierarchyInspectorPanel()
		: mScene(nullptr), mSelected(entt::null), mIsWindowFocused(false)
	{
	}

	SceneHierarchyInspectorPanel::~SceneHierarchyInspectorPanel()
	{
	}

	void SceneHierarchyInspectorPanel::Render()
	{
		// --------------------------------------Hierarchy window----------------------------------------------
		ImGui::Begin("Scene Hierarchy", (bool*)0, ImGuiWindowFlags_NoCollapse);

		mIsWindowFocused = ImGui::IsWindowFocused();

		if (ImGui::Button("Add"))
		{
			mScene->CreateEntity();
		}

		if (ImGui::Button("CreateChild"))
		{
			mScene->CreateChildrenEntity(mSelected);
		}

		const auto& world{ mScene->GetWorld() };
		const auto& view{ world.view<InfoComponent, FamilyComponent>() };

		std::stack<Entity> stack;
		for (const auto& entity : view)
		{
			if (world.get<FamilyComponent>(entity).parent != entt::null)
				continue;

			stack.push(entity);

			while (!stack.empty())
			{
				Entity childEntity{ stack.top() };
				auto [info, family] { world.get<InfoComponent, FamilyComponent>(childEntity)};
				bool select{ mSelected == childEntity };

				ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth };
				if (select)
					flags |= ImGuiTreeNodeFlags_Selected;
				
				bool open{ UI::BeginTreeNode((void*)childEntity, info.name, flags) };
				stack.pop();
				if (open && !family.children.empty())
				{
					for (uint64_t i { family.children.size() - 1U }; i >= 0U; --i)
					{
						stack.push(family.children[i]);
					}
					continue;
				}
				else if (family.parent != entt::null)
					UI::EndTreeNode((void*)childEntity, open);

				if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
					mSelected = childEntity;

				UI::EndTreeNode((void*)childEntity, open);
			}
			
			//bool select = selected == entity;
			//
			//ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			//if (select)
			//	flags |= ImGuiTreeNodeFlags_Selected;
			//bool open = UI::BeginTreeNode((void*)entity, world.get<InfoComponent>(entity).name, flags);
			//
			//if (open && !family.children.empty())
			//{
			//	bool childOpen = UI::BeginTreeNode((void*)entity, world.get<InfoComponent>(family.children[0]).name, flags);
			//	UI::EndTreeNode((void*)entity, childOpen);
			//}
			//
			//if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
			//	selected = entity;
			//
			//UI::EndTreeNode((void*)entity, open);
		}

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered())
			mSelected = entt::null;

		if (ImGui::BeginPopupContextWindow("##HierarchyPopUp"))
		{
			if (ImGui::MenuItem("Create Entity"))
			{
				mScene->CreateEntity();
			}
			if (mSelected != entt::null && ImGui::MenuItem("Delete Entity"))
			{
				mScene->DestroyEntity(mSelected);
				mSelected = entt::null;
			}
			ImGui::EndPopup();
		}

		ImGui::End();
		// ----------------------------------------------------------------------------------------------------

		// --------------------------------------Inspector window----------------------------------------------
		ImGui::Begin("Inspector", (bool*)0);

		if (mSelected != entt::null)
			DrawInspector();

		ImGui::End();

		if (mSelected != entt::null && mIsWindowFocused && Input::IsKeyPressed(KeyCode::DELETE))
		{
			mScene->DestroyEntity(mSelected);
			mSelected = entt::null;
		}
		// ----------------------------------------------------------------------------------------------------
	}
	
	void SceneHierarchyInspectorPanel::DrawInspector()
	{
		auto& world{ mScene->GetWorld() };

		ImGui::PushStyleColor(ImGuiCol_Header, { 12.0f / 255.0f, 12.0f / 255.0f, 25.0f / 255.0f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { 12.0f / 255.0f, 12.0f / 255.0f, 25.0f / 255.0f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, { 12.0f / 255.0f, 12.0f / 255.0f, 25.0f / 255.0f, 1.0f });
		ImGui::Separator();

		if (world.try_get<InfoComponent>(mSelected))
		{
			InfoComponent& infoComponent{ mScene->GetWorld().get<InfoComponent>(mSelected) };
			char* buf{ infoComponent.name.data() };
			ImGui::InputText("##Name", buf, infoComponent.name.size() + 2);
		}

		ImGui::SameLine();

		if (ImGui::BeginCombo("##Add Component", "Add Component"))
		{
			if (ImGui::Selectable("Directional Light Component") && !mScene->HasComponent<DirectionalLightComponent>(mSelected))
			{
				mScene->CreateComponent<DirectionalLightComponent>(mSelected);
			}
			if (ImGui::Selectable("Point Light Component") && !mScene->HasComponent<PointLightComponent>(mSelected))
			{
				mScene->CreateComponent<PointLightComponent>(mSelected);
			}
			//if (ImGui::Selectable("Spot Light Component") && mScene->HasComponent<SpotLightComponent>(mSelected))
			//{
			//	mScene->CreateComponent<SpotLightComponent>(mSelected);
			//}
			ImGui::EndPopup();
		}

		if (world.try_get<TransformComponent>(mSelected))
		{
			TransformComponent& transformComponent{ mScene->GetWorld().get<TransformComponent>(mSelected) };
			if (ImGui::CollapsingHeader("Transform Component"))
			{
				ImVec2 size{ ImGui::CalcTextSize("Rotation") };
				float widthWindow{ ImGui::GetContentRegionMax().x - size.x };
				bool hasChanged{ false };

				ImGui::Text("Position");
				ImGui::SameLine();
				hasChanged |= ImGui::DragFloat3("##Position", glm::value_ptr(transformComponent.position));

				ImGui::Text("Rotation");
				ImGui::SameLine();
				hasChanged |= ImGui::DragFloat3("##Rotation", glm::value_ptr(transformComponent.rotation));

				ImGui::Text("Scale");
				ImGui::SameLine();
				hasChanged |= ImGui::DragFloat3("##Scale", glm::value_ptr(transformComponent.scale));

				if (hasChanged)
					transformComponent.CalculateTransform();
			}
			ImGui::Separator();
		}

		if (world.try_get<StaticMeshComponent>(mSelected))
		{
			StaticMeshComponent& staticMesh{ mScene->GetWorld().get<StaticMeshComponent>(mSelected) };
			if (ImGui::CollapsingHeader("Static Mesh Component"))
			{
				if (auto mesh{ staticMesh.staticMesh.lock() })
				{
					auto material{ staticMesh.material.lock() };

					if (material)
					{
						ImVec4 colorBg { 0.43f, 0.43f, 0.50f, 0.50f };
						ImGui::PushStyleColor(ImGuiCol_ChildBg, colorBg);
						ImGui::PushStyleColor(ImGuiCol_Border, colorBg);

						//ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
						ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 2.0f, 2.0f });
						if (ImGui::BeginChild("Diffuse", { 36, 36 }, true, ImGuiWindowFlags_NoDecoration))
						{
							if (material->GetDiffuse())
							{
								ImGui::Image((void*)material->GetDiffuse()->GetID(), { 32, 32 });
							}
							else
							{
								ImGui::Text("Diffuse");
							}
							if (ImGui::BeginDragDropTarget())
							{
								auto payload{ ImGui::AcceptDragDropPayload("RESOURCE") };

								if (payload)
								{
									UUID uuid{ *(UUID*)payload->Data };
									const auto& metadata{ ResourceManager::GetResourcesMetadata()[uuid] };

									material->UpdateDiffuse(std::static_pointer_cast<Texture2D>(ResourceManager::GetResource(uuid)));
								}
							}
							ImGui::EndChild();
						}
						if (ImGui::ColorEdit4("Color", glm::value_ptr(material->GetMaterialData().color)))
						{
							material->UpdateMaterialData();
						}
						if (ImGui::SliderFloat("Metallic", &material->GetMaterialData().metallic, 0.0f, 1.0f))
						{
							material->UpdateMaterialData();
						}
						if (ImGui::SliderFloat("Roughness", &material->GetMaterialData().roughness, 0.0f, 1.0f))
						{
							material->UpdateMaterialData();
						}
						ImGui::PopStyleColor(2);
						ImGui::PopStyleVar(1);
					}
				}
				ImGui::Separator();
			}	
		}

		if (world.try_get<DirectionalLightComponent>(mSelected))
		{
			DirectionalLightComponent& dirLight{ mScene->GetWorld().get<DirectionalLightComponent>(mSelected) };
			if (ImGui::CollapsingHeader("Directional Light Component"))
			{
				ImGui::Text("Radiance");
				ImGui::SameLine();
				ImGui::ColorEdit3("##Color", glm::value_ptr(dirLight.radiance));
			}
			ImGui::Separator();
		}

		if (world.try_get<PointLightComponent>(mSelected))
		{
			PointLightComponent& pointLight{ mScene->GetWorld().get<PointLightComponent>(mSelected) };
			if (ImGui::CollapsingHeader("Point Light Component"))
			{
				ImGui::Text("Radiance");
				ImGui::SameLine();
				ImGui::ColorEdit3("##Color", glm::value_ptr(pointLight.radiance));

				ImGui::Text("Radius");
				ImGui::SameLine();
				ImGui::SliderFloat("##Radius", &pointLight.radius, 0.2f, 10.0f);
			}
			ImGui::Separator();
		}

		ImGui::PopStyleColor(3);
	}
}