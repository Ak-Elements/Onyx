#include <editor/panels/sceneeditor/entitiespanel.h>

#include <onyx/gamecore/scene/scene.h>

#include <onyx/entity/entityregistry.h>
#include <onyx/gamecore/components/idcomponent.h>
#include <onyx/gamecore/components/namecomponent.h>
#include <onyx/gamecore/components/transformcomponent.h>

#include <editor/modules/sceneeditor.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <onyx/ui/imguistyle.h>
#include <onyx/ui/widgets.h>
#include <imgui_internal.h>

namespace Onyx::Editor::SceneEditor
{
    void EntitiesPanel::Render(GameCore::Scene& scene)
    {
        if ((selectedEntity != entt::null) &&
            ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete, false))
        {
            DeleteEntity(selectedEntity);
        }

        Ui::ScopedImGuiColor styleColor
        {
            { ImGuiCol_TableRowBg, 0 },
            { ImGuiCol_Separator, 0xFFFF0000 }
        };
        const onyxF32 rowHeight = 21.0f;

        if (ImGui::BeginTable("##scene", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None, 0.7f);
            ImGui::TableSetupColumn("Visibility", ImGuiTableColumnFlags_None, 0.3f);

            ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
            ImGui::TableSetColumnIndex(0);
            ImGui::TableHeader("Name");

            ImGui::TableSetColumnIndex(1);
            ImGui::TableHeader("Visibility");
            Entity::EntityRegistry& registry = scene.GetRegistry();

            // TODO: sorting
            /*registry.GetRegistry().sort<Entity::IdComponent>([](const Entity::EntityId lhs, const Entity::EntityId rhs) {
                return lhs < rhs;
                });
            */

            auto entitiesView = registry.GetRegistry().view<GameCore::IdComponent, GameCore::NameComponent>();

            for (Entity::EntityId entity : entitiesView)
            {
                bool isSelected = selectedEntity == entity;

                ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
                ImGui::TableNextColumn();

                const ImVec2 rowAreaMin = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 0).Min;
                const ImVec2 rowAreaMax = { ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), ImGui::TableGetColumnCount() - 1).Max.x,
                                            rowAreaMin.y + rowHeight };


                Ui::ScopedImGuiId scopedId(Format::Format("Entity {}", static_cast<onyxU64>(entity)));

                ImGui::PushClipRect(rowAreaMin, rowAreaMax, false);
                ImGuiID id = ImGui::GetID("rowSelection");
                bool isRowHovered = false, isRowClicked = false, held = false;
                isRowClicked |= ImGui::ButtonBehavior(ImRect(rowAreaMin, rowAreaMax), id,
                    &isRowHovered, &held, ImGuiButtonFlags_AllowOverlap | ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
                ImGui::KeepAliveID(id);
                ImGui::PopClipRect();

                const GameCore::NameComponent& nameComponent = entitiesView.get<GameCore::NameComponent>(entity);
                String entityName = nameComponent.Name;

                {
                    Ui::ScopedImGuiColor renameStyleColor
                    {
                        { ImGuiCol_Header, 0x0 },
                        { ImGuiCol_HeaderActive, 0x0 },
                        { ImGuiCol_HeaderHovered, 0x0 }
                    };

                    Ui::ScopedImGuiStyle renameStyle
                    {
                        { ImGuiStyleVar_FramePadding, ImVec2(0, 0)},
                        { ImGuiStyleVar_SelectableTextAlign, ImVec2(0,0.5f)},
                    };

                    ImGui::SetNextItemAllowOverlap();
                    if (Ui::DrawRenameInput("name", entityName, ImVec2(-1, 0), isSelected))
                    {
                        registry.AddComponent<GameCore::NameComponent>(selectedEntity, entityName);
                    }

                    isRowClicked |= isSelected;
                    isRowHovered |= ImGui::IsItemHovered();
                }

                if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonRight))
                {
                    if (ImGui::MenuItem("Delete"))
                    {
                        DeleteEntity(entity);
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }


                ImGui::TableNextColumn();

                //bool load = false;
                /*ImGui::SetNextItemAllowOverlap();
                ImGui::BeginHorizontal("##visibilityToggles");
                ImGui::Checkbox("##load", &load);
                ImGui::EndHorizontal();*/

                if (isRowClicked)
                {
                    SetSelectedEntity(entity);
                }

                onyxU32 cellBackgroundColor = isRowHovered ? ImGui::GetColorU32(ImGuiCol_HeaderHovered) : (isSelected ? ImGui::GetColorU32(ImGuiCol_HeaderActive) : ImGui::GetColorU32(ImGuiTableBgTarget_CellBg));
                for (int column = 0; column < ImGui::TableGetColumnCount(); column++)
                {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cellBackgroundColor, column);
                }
            }

            if (ImGui::BeginPopupContextWindow("CreateEntityPopUp", ImGuiPopupFlags_MouseButtonRight))
            {
                if (ImGui::MenuItem("Create Entity"))
                {
                    Entity::EntityId createdEntity = registry.CreateEntity();
                    registry.AddComponent<GameCore::IdComponent>(createdEntity);
                    registry.AddComponent<GameCore::NameComponent>(createdEntity, GetNewEntityName());
                    registry.AddComponent<GameCore::TransformComponent>(createdEntity);
                    SetSelectedEntity(createdEntity);

                    ImGui::CloseCurrentPopup();
                }

                if (selectedEntity != entt::null)
                {
                    if (ImGui::MenuItem("Duplicate"))
                    {
                        selectedEntity = registry.DuplicateEntity(selectedEntity);
                        ImGui::CloseCurrentPopup();
                    }
                }

                ImGui::EndPopup();
            }

            ImGui::EndTable();
        }
    }


    String EntitiesPanel::GetNewEntityName() const
    {
        // TODO: Add logic to have unique names in scene
        String entityName = "New Entity";
        return entityName;
    }

    void EntitiesPanel::DeleteEntity(Entity::EntityId /*entity*/)
    {
        /*if (entity == selectedEntity)
        {
            SetSelectedEntity(entt::null);
            m_OnSelectedEntityChanged(entity);
        }

        scene->GetRegistry().DeleteEntity(entity);*/
    }

    void EntitiesPanel::SetSelectedEntity(Entity::EntityId /*entity*/)
    {
        /*if (selectedEntity != entity)
        {
            Entity::EntityRegistry& registry = scene->GetRegistry();
            if (selectedEntity != entt::null)
                registry.RemoveComponent<SelectedComponent>(selectedEntity);

            selectedEntity = entity;

            registry.AddComponent<SelectedComponent>(selectedEntity);

            m_OnSelectedEntityChanged(selectedEntity);
        }*/
    }
}
