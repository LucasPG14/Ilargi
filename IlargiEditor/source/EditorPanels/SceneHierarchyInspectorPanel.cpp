#include "ilargipch.h"

#include "SceneHierarchyInspectorPanel.h"
#include "Utils/UI/IlargiUI.h"
#include "Base/Input.h"

#include "Resources/ResourceManager.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Resources/Texture.h"

#include <imgui/imgui.h>

namespace Ilargi
{
	SceneHierarchyInspectorPanel::SceneHierarchyInspectorPanel(const std::shared_ptr<Scene>& actualScene)
		: scene(actualScene), selected(entt::null)
	{
	}

	SceneHierarchyInspectorPanel::~SceneHierarchyInspectorPanel()
	{
	}

	void SceneHierarchyInspectorPanel::Render()
	{
		// --------------------------------------Hierarchy window----------------------------------------------
		ImGui::Begin("Scene Hierarchy", (bool*)0, ImGuiWindowFlags_NoCollapse);

		if (ImGui::Button("Add"))
		{
			scene->CreateEntity();
		}

		if (ImGui::Button("CreateChild"))
		{
			scene->CreateChildrenEntity(selected);
		}

		const auto& world = scene->GetWorld();
		const auto& view = world.view<InfoComponent, FamilyComponent>();

		std::stack<Entity> stack;
		for (const auto& entity : view)
		{
			if (world.get<FamilyComponent>(entity).parent != entt::null)
				continue;

			stack.push(entity);

			while (!stack.empty())
			{
				Entity childEntity = stack.top();
				auto [info, family] = world.get<InfoComponent, FamilyComponent>(childEntity);
				bool select = selected == childEntity;

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
				if (select)
					flags |= ImGuiTreeNodeFlags_Selected;
				
				bool open = UI::BeginTreeNode((void*)childEntity, info.name, flags);
				stack.pop();
				if (open && !family.children.empty())
				{
					for (int i = family.children.size() - 1; i >= 0; --i)
					{
						stack.push(family.children[i]);
					}
					continue;
				}
				else if (family.parent != entt::null)
					UI::EndTreeNode((void*)childEntity, open);

				if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
					selected = childEntity;

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
			selected = entt::null;

		if (ImGui::BeginPopupContextWindow("##HierarchypopUp"))
		{
			if (ImGui::MenuItem("Create Entity"))
			{
				scene->CreateEntity();
			}
			if (selected != entt::null && ImGui::MenuItem("Delete Entity"))
			{
				scene->DestroyEntity(selected);
				selected = entt::null;
			}
			ImGui::EndPopup();
		}

		ImGui::End();
		// ----------------------------------------------------------------------------------------------------

		// --------------------------------------Inspector window----------------------------------------------
		ImGui::Begin("Inspector", (bool*)0);

		if (selected != entt::null)
			DrawInspector();

		ImGui::End();

		if (selected != entt::null && Input::IsKeyPressed(KeyCode::DELETE))
		{
			scene->DestroyEntity(selected);
			selected = entt::null;
		}
		// ----------------------------------------------------------------------------------------------------
	}
	
	void SceneHierarchyInspectorPanel::DrawInspector()
	{
		ImGui::PushStyleColor(ImGuiCol_Header, { 12.0f / 255.0f, 12.0f / 255.0f, 25.0f / 255.0f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { 12.0f / 255.0f, 12.0f / 255.0f, 25.0f / 255.0f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, { 12.0f / 255.0f, 12.0f / 255.0f, 25.0f / 255.0f, 1.0f });
		ImGui::Separator();

		auto& world = scene->GetWorld();
		if (world.try_get<TransformComponent>(selected))
		{
			TransformComponent& transformComponent = scene->GetWorld().get<TransformComponent>(selected);
			if (ImGui::CollapsingHeader("Transform Component"))
			{
				ImVec2 size = ImGui::CalcTextSize("Rotation");
				float widthWindow = ImGui::GetContentRegionMax().x - size.x;

				ImGui::Text("Position");
				ImGui::SameLine();
				ImGui::DragFloat3("##Position", transformComponent.position);

				ImGui::Text("Rotation");
				ImGui::SameLine();
				ImGui::DragFloat3("##Rotation", transformComponent.rotation);

				ImGui::Text("Scale");
				ImGui::SameLine();
				ImGui::DragFloat3("##Scale", transformComponent.scale);
			}
			ImGui::Separator();
		}

		if (world.try_get<StaticMeshComponent>(selected))
		{
			StaticMeshComponent& staticMesh = scene->GetWorld().get<StaticMeshComponent>(selected);
			if (ImGui::CollapsingHeader("Static Mesh Component"))
			{
				//ImGui::ColorPicker4("##Color", staticMesh.staticMesh->GetColor());
				auto& material = staticMesh.staticMesh->GetMaterial();

				if (material->GetDiffuse())
				{
					ImGui::Image((void*)material->GetDiffuse()->GetID(), { 64, 64 });
				}
				else
				{
					ImGui::Text("Diffuse");
				}
				if (ImGui::BeginDragDropTarget())
				{
					auto payload = ImGui::AcceptDragDropPayload("RESOURCE");

					if (payload)
					{
						UUID uuid = *(UUID*)payload->Data;
						auto metadata = ResourceManager::GetResourcesMetadata()[uuid];

						material->SetDiffuse(std::static_pointer_cast<Texture2D>(ResourceManager::GetResource(uuid)));
					}
				}
			}
			ImGui::Separator();
		}

		if (world.try_get<DirectionalLightComponent>(selected))
		{
			DirectionalLightComponent& dirLight = scene->GetWorld().get<DirectionalLightComponent>(selected);
			if (ImGui::CollapsingHeader("Directional Light Component"))
			{
				ImGui::ColorPicker4("##Color", dirLight.radiance);
			}
			ImGui::Separator();
		}

		ImGui::PopStyleColor(3);
	}
}