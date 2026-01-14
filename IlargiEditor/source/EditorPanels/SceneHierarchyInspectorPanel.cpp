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
#include "../LocalizationManager.h"

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
		ImGui::Begin(LOC("editor.scenehierarchy"), (bool*)0, ImGuiWindowFlags_NoCollapse);

		mIsWindowFocused = ImGui::IsWindowFocused();

		if (ImGui::Button(LOC("editor.scenehierarchy.add")))
		{
			mScene->CreateEntity();
		}

		if (ImGui::Button(LOC("editor.scenehierarchy.createchild")))
		{
			mScene->CreateChildrenEntity(mSelected);
		}

		const auto& world{ mScene->GetWorld() };
		const auto& view{ world.view<TransformComponent, InfoComponent>() };

		std::stack<Entity> stack;
		for (const auto& entity : view)
		{
			if (mScene->HasComponent<ParentComponent>(entity))
				continue;
		
			DrawNode(entity, world);
		}

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered())
			mSelected = entt::null;

		if (ImGui::BeginPopupContextWindow("##HierarchyPopUp"))
		{
			if (ImGui::MenuItem(LOC("editor.scenehierarchy.createentity")))
			{
				mScene->CreateEntity();
			}
			if (mSelected != entt::null && ImGui::MenuItem(LOC("editor.scenehierarchy.deleteentity")))
			{
				mScene->DestroyEntity(mSelected);
				mSelected = entt::null;
			}
			ImGui::EndPopup();
		}

		ImGui::End();
		// ----------------------------------------------------------------------------------------------------

		// --------------------------------------Inspector window----------------------------------------------
		ImGui::Begin(LOC("editor.inspector"), (bool*)0);

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
			InfoComponent& infoComponent{ mScene->GetComponent<InfoComponent>(mSelected) };
			char* buf{ infoComponent.name.data() };
			ImGui::InputText("##Name", buf, infoComponent.name.size() + 2);
		}

		ImGui::SameLine();

		if (ImGui::BeginCombo("##Add Component", LOC("editor.inspector.addcomponent")))
		{
			if (ImGui::Selectable(LOC("editor.inspector.directionallight")) && !mScene->HasComponent<DirectionalLightComponent>(mSelected))
			{
				mScene->CreateComponent<DirectionalLightComponent>(mSelected);
			}
			if (ImGui::Selectable(LOC("editor.inspector.pointlight")) && !mScene->HasComponent<PointLightComponent>(mSelected))
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
			TransformComponent& transformComponent{ mScene->GetComponent<TransformComponent>(mSelected) };
			if (ImGui::CollapsingHeader(LOC("editor.inspector.transform")))
			{
				ImVec2 size{ ImGui::CalcTextSize(LOC("editor.transform.rotation")) };
				float widthWindow{ ImGui::GetContentRegionMax().x - size.x };
				bool hasChanged{ false };

				ImGui::Text(LOC("editor.transform.position"));
				ImGui::SameLine();
				hasChanged |= ImGui::DragFloat3("##Position", glm::value_ptr(transformComponent.position));

				ImGui::Text(LOC("editor.transform.rotation"));
				ImGui::SameLine();
				hasChanged |= ImGui::DragFloat3("##Rotation", glm::value_ptr(transformComponent.rotation));

				ImGui::Text(LOC("editor.transform.scale"));
				ImGui::SameLine();
				hasChanged |= ImGui::DragFloat3("##Scale", glm::value_ptr(transformComponent.scale));

				if (hasChanged)
				{
					transformComponent.CalculateTransform();
					if (mScene->HasComponent<ParentComponent>(mSelected))
					{
						const ParentComponent& parentComponent{ mScene->GetComponent<ParentComponent>(mSelected) };
						TransformComponent& parentTransformComponent{ mScene->GetComponent<TransformComponent>(parentComponent.parent) };
						transformComponent.CalculateWorldTransform(parentTransformComponent.worldTransform);
					}
					else
					{
						transformComponent.CalculateWorldTransform(glm::mat4(1.0));
					}

					if (mScene->HasComponent<ChildComponent>(mSelected))
					{
						mScene->CalculateChildrenTransforms(mSelected, transformComponent.worldTransform);
					}
				}
			}
			ImGui::Separator();
		}

		if (world.try_get<StaticMeshComponent>(mSelected))
		{
			StaticMeshComponent& staticMesh{ mScene->GetComponent<StaticMeshComponent>(mSelected) };
			if (ImGui::CollapsingHeader(LOC("editor.inspector.staticmesh")))
			{
				for (auto& submesh : staticMesh.submeshes)
				{
					ImGui::Text(ResourceManager::GetMetadata(submesh.mesh).filepath.filename().stem().string().c_str());
					ImGui::Text(ResourceManager::GetMetadata(submesh.material).filepath.filename().stem().string().c_str());
					if (ImGui::BeginDragDropTarget())
					{
						auto payload{ ImGui::AcceptDragDropPayload("MATERIAL") };

						if (payload)
						{
							UUID uuid{ *(UUID*)payload->Data };
							submesh.material = uuid;
						}
					}
					ImGui::Separator();
				}
			}	
		}

		if (world.try_get<DirectionalLightComponent>(mSelected))
		{
			DirectionalLightComponent& dirLight{ mScene->GetComponent<DirectionalLightComponent>(mSelected) };
			if (ImGui::CollapsingHeader(LOC("editor.inspector.directionallight")))
			{
				ImGui::Text("Radiance");
				ImGui::SameLine();
				ImGui::ColorEdit3("##Color", glm::value_ptr(dirLight.radiance));
			}
			ImGui::Separator();
		}

		if (world.try_get<PointLightComponent>(mSelected))
		{
			PointLightComponent& pointLight{ mScene->GetComponent<PointLightComponent>(mSelected) };
			if (ImGui::CollapsingHeader(LOC("editor.inspector.pointlight")))
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
	
	void SceneHierarchyInspectorPanel::DrawNode(const Entity aEntity, const entt::registry& aWorld)
	{
		const InfoComponent& infoComponent { aWorld.get<InfoComponent>(aEntity)};
		bool select{ mSelected == aEntity };

		ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth };
		if (select)
			flags |= ImGuiTreeNodeFlags_Selected;

		bool open{ UI::BeginTreeNode((void*)aEntity, infoComponent.name, flags) };
		if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
			mSelected = aEntity;

		if (open && mScene->HasComponent<ChildComponent>(aEntity))
		{
			const ChildComponent& childComponent{ mScene->GetComponent<ChildComponent>(aEntity) };
			for (const auto& entityChild : childComponent.childrens)
			{
				DrawNode(entityChild, aWorld);
			}
		}

		UI::EndTreeNode((void*)aEntity, open);
	}
}