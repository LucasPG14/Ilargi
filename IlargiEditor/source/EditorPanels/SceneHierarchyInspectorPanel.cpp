#include "ilargipch.h"

#include "SceneHierarchyInspectorPanel.h"
#include "Utils/UI/IlargiUI.h"
#include "Base/Input.h"

#include "Resources/Mesh.h"

#include <imgui/imgui.h>

namespace Ilargi
{
	SceneHierarchyInspectorPanel::SceneHierarchyInspectorPanel(std::shared_ptr<Scene> actualScene)
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

		const auto& world = scene->GetWorld();
		auto iterator = world.storage<InfoComponent>()->reach();

		auto begIterator = world.storage<InfoComponent>()->rbegin();
		auto endIterator = world.storage<InfoComponent>()->rend();

		for (const auto& iterate : iterator)
		{
			entt::entity entity = iterate._Myfirst._Val;
			bool select = selected == entity;

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			if (select)
			{
				//ImGui::PushStyleColor(ImGuiCol_Header, { 1.0f, 0.0f, 0.0f, 0.30f });
				flags |= ImGuiTreeNodeFlags_Selected;
			}
			bool open = UI::BeginTreeNode((void*)entity, world.get<InfoComponent>(entity).name, flags);

			if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
				selected = entity;

			UI::EndTreeNode((void*)entity, open);
			//if (select)
			//	ImGui::PopStyleColor();
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
				ImGui::DragFloat3("Position", transformComponent.position);
				ImGui::DragFloat3("Rotation", transformComponent.rotation);
				ImGui::DragFloat3("Scale", transformComponent.scale);
			}
			ImGui::Separator();
		}

		if (world.try_get<StaticMeshComponent>(selected))
		{
			StaticMeshComponent& staticMesh = scene->GetWorld().get<StaticMeshComponent>(selected);
			if (ImGui::CollapsingHeader("Static Mesh Component"))
			{
				ImGui::ColorPicker4("##Color", staticMesh.staticMesh->GetColor());
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