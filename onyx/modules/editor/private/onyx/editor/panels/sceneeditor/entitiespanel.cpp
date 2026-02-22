#include <onyx/editor/panels/sceneeditor/entitiespanel.h>

#include <onyx/gamecore/scene/scene.h>
#include <onyx/gamecore/gamecore.h>

#include <onyx/entity/entityregistry.h>
#include <onyx/gamecore/components/idcomponent.gen.h>
#include <onyx/gamecore/components/namecomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>

#include <onyx/editor/windows/sceneeditor.h>
#include <onyx/editor/editor_localization.h>

#include <onyx/editor/commands/commandgraph.h>
#include <onyx/editor/commands/scene/createentitycommand.h>
#include <onyx/editor/commands/scene/deleteentitycommand.h>
#include <onyx/editor/commands/scene/renameentitycommand.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <onyx/ui/scopedcolor.h>
#include <onyx/ui/scopedid.h>
#include <onyx/ui/scopedstyle.h>
#include <onyx/ui/widgets.h>
#include <imgui_internal.h>
#include <onyx/localization/localization.h>
#include <onyx/ui/controls/button.h>

namespace Onyx::Editor::SceneEditor
{
    void EntitiesPanel::OnOpen()
    {
        InputActions::InputActionSystem& inputActionSystem = GetEngineSystem<InputActions::InputActionSystem>();
        inputActionSystem.OnInput<&EntitiesPanel::OnDeleteAction>("Delete"_id64, this);
    }

    void EntitiesPanel::OnClose()
    {
        InputActions::InputActionSystem& inputActionSystem = GetEngineSystem<InputActions::InputActionSystem>();
        inputActionSystem.Disconnect(this);
    }

    void EntitiesPanel::OnRender(Ui::ImGuiSystem& /*imguiSystem*/)
    {
        ONYX_ASSERT(m_CommandGraph != nullptr);

        SceneEditorWindow& parent = *(GetParent<SceneEditorWindow>().value());

        if (parent.IsLoading())
            return;

        SetName(String(GetWindowId()));
        Begin();
        {
        //GameCore::GameCoreSystem& gameCoreSystem, Assets::AssetId sceneId, GameCore::Scene& scene, ICommandGraph& commandStack
        Ui::ScopedImGuiColor styleColor
        {
            { ImGuiCol_TableRowBg, 0x0 },
            { ImGuiCol_Separator, 0xFFFF0000 },
            { ImGuiCol_NavCursor, 0x0 }
        };
        const onyxF32 rowHeight = 21.0f;

        if (ImGui::BeginTable("##scene", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None, 0.7f);
            ImGui::TableSetupColumn("Visibility", ImGuiTableColumnFlags_None, 0.3f);

            ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
            ImGui::TableSetColumnIndex(0);
            ImGui::TableHeader(Localization::Generic::Name.Get().data());

            ImGui::TableSetColumnIndex(1);
            ImGui::TableHeader(Localization::Generic::Visibility.Get().data());

            Entity::EntityRegistry& registry = parent.GetScene().GetRegistry();

            // TODO: sorting
            /*registry.GetRegistry().sort<Entity::IdComponent>([](const Entity::EntityId lhs, const Entity::EntityId rhs) {
                return lhs < rhs;
                });
            */

            auto entitiesView = registry.GetRegistry().view<GameCore::IdComponent, GameCore::NameComponent>();

            for (Entity::EntityId entity : entitiesView)
            {
                bool isSelected = m_SelectedEntity == entity;

                if ( isSelected && registry.HasComponents<SelectedComponent>( m_SelectedEntity ) == false)
                {
                    registry.AddComponent<SelectedComponent>( m_SelectedEntity );
                }

                ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
                ImGui::TableNextColumn();

                const ImVec2 rowAreaMin = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 0).Min;
                const ImVec2 rowAreaMax = { ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), ImGui::TableGetColumnCount() - 1).Max.x,
                                            rowAreaMin.y + rowHeight };

                Ui::ScopedImGuiId scopedId(Format::Format("Entity {}", static_cast<onyxU64>(entity)));

                ImGui::PushClipRect(rowAreaMin, rowAreaMax, false);
                ImGuiID id = ImGui::GetID("rowSelection");
                Ui::ButtonState state = Ui::ButtonBehavior(id, ImRect(rowAreaMin, rowAreaMax), ImGuiButtonFlags_AllowOverlap | ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
                bool isRowClicked = state == Ui::ButtonState::Pressed;
                bool isRowHovered = state == Ui::ButtonState::Hovered;
                bool isRowFocused = false;
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
                    String previousName = entityName;
                    if (Ui::DrawRenameInput("name", entityName, ImVec2(-1, 0), isSelected))
                    {
                        m_CommandGraph->Push<RenameEntityCommand>(m_SelectedEntity, entityName, parent.GetSceneId(), GetEngineSystem<GameCore::GameCoreSystem>()); 
                    }

                    isRowClicked |= isSelected;
                    isRowHovered |= ImGui::IsItemHovered();
                    isRowFocused = ImGui::IsItemFocused();
                }

                if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonRight))
                {
                    if (ImGui::MenuItem(Localization::Generic::Delete.Get().data()))
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

                if (isRowClicked || isRowFocused)
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
                if (ImGui::MenuItem(Localization::Generic::Create.Get().data()))
                {
                    m_CommandGraph->Push<CreateEntityCommand>( parent.GetSceneId(), GetEngineSystem<GameCore::GameCoreSystem>() );
                    ImGui::CloseCurrentPopup();
                }

                if (m_SelectedEntity != Entity::EntityId::Invalid)
                {
                    if (ImGui::MenuItem(Localization::Generic::Duplicate.Get().data()))
                    {
                       // selectedEntity = registry.DuplicateEntity(selectedEntity);
                        ImGui::CloseCurrentPopup();
                    }
                }

                ImGui::EndPopup();
            }

            ImGui::EndTable();
        }
        }
        End();
    }

    String EntitiesPanel::GetNewEntityName() const
    {
        // TODO: Add logic to have unique names in scene
        String entityName = "New Entity";
        return entityName;
    }

    void EntitiesPanel::OnDeleteAction(const InputActions::InputActionEvent& deleteAction)
    {
        if (IsFocused() == false)
            return;

        if (m_SelectedEntity == Entity::EntityId::Invalid)
            return;
        
        if (deleteAction.GetData<bool>() == false)
            return;

        DeleteEntity(m_SelectedEntity);
    }

    void EntitiesPanel::DeleteEntity(Entity::EntityId entity)
    {
        if (entity == m_SelectedEntity)
        {
            SetSelectedEntity(Entity::EntityId::Invalid);
        }

        SceneEditorWindow& parent = *(GetParent<SceneEditorWindow>().value());
        m_CommandGraph->Push<DeleteEntityCommand>(entity, parent.GetSceneId(), GetEngineSystem<GameCore::GameCoreSystem>());
    }

    void EntitiesPanel::SetSelectedEntity(Entity::EntityId entity)
    {
        if (m_SelectedEntity != entity)
        {
            SceneEditorWindow& parent = *GetParent<SceneEditorWindow>().value();
            
            Entity::EntityRegistry& registry = parent.GetScene().GetRegistry();
            if (m_SelectedEntity != Entity::EntityId::Invalid)
                registry.RemoveComponent<SelectedComponent>(m_SelectedEntity);

            m_SelectedEntity = entity;
            if (m_SelectedEntity != Entity::EntityId::Invalid)
            {
                registry.AddComponent<SelectedComponent>(m_SelectedEntity);
            }
        }
    }
}
