#include "ilargipch.h"

#include "SceneHierarchyInspectorPanel.h"
#include "Utils/IlargiUI.h"

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
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { 1.0f, 0.0f, 0.0f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, { 1.0f, 0.0f, 0.0f, 1.0f });
		ImGui::Begin("Scene Hierarchy", (bool*)0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
		ImGui::BeginChild("##", ImGui::GetContentRegionAvail());

		const auto& world = scene->GetWorld();
		auto iterator = world.storage<InfoComponent>()->each();
		
		for (const auto& iterate : iterator)
		{
			entt::entity entity = iterate._Myfirst._Val;
			bool select = selected == entity;
			
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			if (select)
			{
				ImGui::PushStyleColor(ImGuiCol_Header, { 1.0f, 0.0f, 0.0f, 0.20f });
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

		ImGui::EndChild();
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(2);
		// ----------------------------------------------------------------------------------------------------

		// --------------------------------------Inspector window----------------------------------------------
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::Begin("Inspector", (bool*)0);
		ImGui::BeginChild("##Inspector", ImGui::GetContentRegionAvail());

		if (selected != entt::null)
			DrawInspector();

		ImGui::EndChild();
		ImGui::End();
		ImGui::PopStyleVar();
		// ----------------------------------------------------------------------------------------------------
	}
	
	void SceneHierarchyInspectorPanel::DrawInspector()
	{
		ImGui::Separator();
		auto& world = scene->GetWorld();
		if (world.try_get<TransformComponent>(selected))
		{
			const TransformComponent& transformComponent = scene->GetWorld().get<TransformComponent>(selected);
			UI::BeginCollapsingHeader((void*)selected, "Transform Component");
			UI::EndCollapsingHeader((void*)selected);
		}
		ImGui::Separator();
	}
}