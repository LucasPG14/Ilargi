#include "ilargipch.h"

#include "SceneHierarchyInspectorPanel.h"
#include "Utils/UI/IlargiUI.h"

#include "Resources/Mesh.h"

#include <imgui/imgui.h>
#include <gtc/type_ptr.hpp>

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
				ImGui::PushStyleColor(ImGuiCol_Header, { 1.0f, 0.0f, 0.0f, 0.30f });
				flags |= ImGuiTreeNodeFlags_Selected;
			}
			bool open = UI::BeginTreeNode((void*)entity, world.get<InfoComponent>(entity).name, flags);

			if (ImGui::IsItemClicked(0))
				selected = entity;

			UI::EndTreeNode((void*)entity, open);
			if (select)
				ImGui::PopStyleColor();
		}

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered())
			selected = entt::null;

		ImGui::End();
		// ----------------------------------------------------------------------------------------------------

		// --------------------------------------Inspector window----------------------------------------------
		ImGui::Begin("Inspector", (bool*)0);

		if (selected != entt::null)
			DrawInspector();

		ImGui::End();
		// ----------------------------------------------------------------------------------------------------
	}
	
	void SceneHierarchyInspectorPanel::DrawInspector()
	{
		ImGui::PushStyleColor(ImGuiCol_Header, { 1.0f, 0.0f, 0.0f, 0.15f });
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { 1.0f, 0.0f, 0.0f, 0.30f });
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, { 1.0f, 0.0f, 0.0f, 0.30f });
		ImGui::Separator();
		auto& world = scene->GetWorld();
		if (world.try_get<TransformComponent>(selected))
		{
			TransformComponent& transformComponent = scene->GetWorld().get<TransformComponent>(selected);
			if (ImGui::CollapsingHeader("Transform Component"))
			{
				ImGui::DragFloat3("Position", glm::value_ptr(transformComponent.position));
				ImGui::DragFloat3("Rotation", glm::value_ptr(transformComponent.rotation));
				ImGui::DragFloat3("Scale", glm::value_ptr(transformComponent.scale));
			}
		}
		ImGui::Separator();

		if (world.try_get<TransformComponent>(selected))
		{
			StaticMeshComponent& meshComponent = scene->GetWorld().get<StaticMeshComponent>(selected);
			if (ImGui::CollapsingHeader("Static Mesh Component"))
			{
				ImGui::ColorPicker4("##Color", glm::value_ptr(meshComponent.staticMesh->GetColor()));
			}
		}
		ImGui::Separator();
		ImGui::PopStyleColor(3);
	}
}