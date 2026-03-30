#include <onyx/editor/panels/sceneeditor/entitiespanel.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>

#include <onyx/entity/entityregistry.h>
#include <onyx/gamecore/components/idcomponent.gen.h>
#include <onyx/gamecore/components/namecomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>

#include <onyx/editor/editor_localization.h>
#include <onyx/editor/windows/sceneeditor.h>

#include <onyx/editor/commands/commandgraph.h>
#include <onyx/editor/commands/scene/createentitycommand.h>
#include <onyx/editor/commands/scene/deleteentitycommand.h>
#include <onyx/editor/commands/scene/renameentitycommand.h>

#include <imgui_internal.h>
#include <onyx/localization/localization.h>
#include <onyx/ui/controls/button.h>
#include <onyx/ui/scopedcolor.h>
#include <onyx/ui/scopedid.h>
#include <onyx/ui/scopedstyle.h>
#include <onyx/ui/widgets.h>

namespace onyx::editor::scene_editor {
void EntitiesPanel::onOpen() {
    input_actions::InputActionSystem& inputActionSystem = getEngineSystem< input_actions::InputActionSystem >();
    inputActionSystem.OnInput< &EntitiesPanel::onDeleteAction >( "Delete"_id64, this );
}

void EntitiesPanel::onClose() {
    input_actions::InputActionSystem& inputActionSystem = getEngineSystem< input_actions::InputActionSystem >();
    inputActionSystem.Disconnect( this );
}

void EntitiesPanel::onRender( ui::ImGuiSystem& /*imguiSystem*/ ) {
    ONYX_ASSERT( m_commandGraph != nullptr );

    SceneEditorWindow& parent = *( getParent< SceneEditorWindow >().value() );

    if( parent.isLoading() )
        return;

    // setName( String( getWindowId() ) );
    {
        // game_core::GameCoreSystem& gameCoreSystem, assets::AssetId sceneId, game_core::Scene& scene, ICommandGraph&
        // commandStack
        ui::ScopedImGuiColor styleColor{ { ImGuiCol_TableRowBg, 0x0 },
                                         { ImGuiCol_Separator, 0xFFFF0000 },
                                         { ImGuiCol_NavCursor, 0x0 } };
        const float32 rowHeight = 21.0f;

        if( ImGui::BeginTable( "##scene",
                               2,
                               ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable |
                                   ImGuiTableFlags_SizingStretchProp ) ) {
            ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_None, 0.7f );
            ImGui::TableSetupColumn( "Visibility", ImGuiTableColumnFlags_None, 0.3f );

            ImGui::TableNextRow( ImGuiTableRowFlags_Headers );
            ImGui::TableSetColumnIndex( 0 );
            ImGui::TableHeader( localization::generic::Name.Get().data() );

            ImGui::TableSetColumnIndex( 1 );
            ImGui::TableHeader( localization::generic::Visibility.Get().data() );

            ecs::EntityRegistry& registry = parent.getScene().GetRegistry();

            // TODO: sorting
            /*registry.GetRegistry().sort<ecs::IdComponent>([](const ecs::EntityId lhs, const ecs::EntityId rhs) {
                return lhs < rhs;
                });
            */

            auto entitiesView = registry.GetRegistry().view< game_core::IdComponent, game_core::NameComponent >();

            for( ecs::EntityId entity : entitiesView ) {
                bool isSelected = m_selectedEntity == entity;

                if( isSelected && registry.HasComponents< SelectedComponent >( m_selectedEntity ) == false ) {
                    registry.AddComponent< SelectedComponent >( m_selectedEntity );
                }

                ImGui::TableNextRow( ImGuiTableRowFlags_None, rowHeight );
                ImGui::TableNextColumn();

                const ImVec2 rowAreaMin = ImGui::TableGetCellBgRect( ImGui::GetCurrentTable(), 0 ).Min;
                const ImVec2 rowAreaMax = {
                    ImGui::TableGetCellBgRect( ImGui::GetCurrentTable(), ImGui::TableGetColumnCount() - 1 ).Max.x,
                    rowAreaMin.y + rowHeight };

                ui::ScopedImGuiId scopedId( format::format( "Entity {}", static_cast< uint64_t >( entity ) ) );

                ImGui::PushClipRect( rowAreaMin, rowAreaMax, false );
                ImGuiID id = ImGui::GetID( "rowSelection" );
                ui::ButtonState state = ui::ButtonBehavior(
                    id,
                    ImRect( rowAreaMin, rowAreaMax ),
                    ImGuiButtonFlags_AllowOverlap | ImGuiButtonFlags_PressedOnClickRelease |
                        ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight );
                bool isRowClicked = state == ui::ButtonState::Pressed;
                bool isRowHovered = state == ui::ButtonState::Hovered;
                bool isRowFocused = false;
                ImGui::KeepAliveID( id );
                ImGui::PopClipRect();

                const game_core::NameComponent& nameComponent = entitiesView.get< game_core::NameComponent >( entity );
                String entityName = nameComponent.Name;

                {
                    ui::ScopedImGuiColor renameStyleColor{ { ImGuiCol_Header, 0x0 },
                                                           { ImGuiCol_HeaderActive, 0x0 },
                                                           { ImGuiCol_HeaderHovered, 0x0 } };

                    ui::ScopedImGuiStyle renameStyle{
                        { ImGuiStyleVar_FramePadding, ImVec2( 0, 0 ) },
                        { ImGuiStyleVar_SelectableTextAlign, ImVec2( 0, 0.5f ) },
                    };

                    ImGui::SetNextItemAllowOverlap();
                    String previousName = entityName;
                    if( ui::DrawRenameInput( "name", entityName, ImVec2( -1, 0 ), isSelected ) ) {
                        m_commandGraph->Push< RenameEntityCommand >( m_selectedEntity,
                                                                     entityName,
                                                                     parent.getSceneId(),
                                                                     getEngineSystem< game_core::GameCoreSystem >() );
                    }

                    isRowClicked |= isSelected;
                    isRowHovered |= ImGui::IsItemHovered();
                    isRowFocused = ImGui::IsItemFocused();
                }

                if( ImGui::BeginPopupContextItem( nullptr, ImGuiPopupFlags_MouseButtonRight ) ) {
                    if( ImGui::MenuItem( localization::generic::Delete.Get().data() ) ) {
                        deleteEntity( entity );
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }

                ImGui::TableNextColumn();

                // bool load = false;
                /*ImGui::SetNextItemAllowOverlap();
                ImGui::BeginHorizontal("##visibilityToggles");
                ImGui::Checkbox("##load", &load);
                ImGui::EndHorizontal();*/

                if( isRowClicked || isRowFocused ) {
                    setSelectedEntity( entity );
                }

                uint32_t cellBackgroundColor = isRowHovered
                                                   ? ImGui::GetColorU32( ImGuiCol_HeaderHovered )
                                                   : ( isSelected ? ImGui::GetColorU32( ImGuiCol_HeaderActive )
                                                                  : ImGui::GetColorU32( ImGuiTableBgTarget_CellBg ) );
                for( int column = 0; column < ImGui::TableGetColumnCount(); column++ ) {
                    ImGui::TableSetBgColor( ImGuiTableBgTarget_CellBg, cellBackgroundColor, column );
                }
            }

            if( ImGui::BeginPopupContextWindow( "CreateEntityPopUp", ImGuiPopupFlags_MouseButtonRight ) ) {
                if( ImGui::MenuItem( localization::generic::Create.Get().data() ) ) {
                    m_commandGraph->Push< CreateEntityCommand >( parent.getSceneId(),
                                                                 getEngineSystem< game_core::GameCoreSystem >() );
                    ImGui::CloseCurrentPopup();
                }

                if( m_selectedEntity != ecs::EntityId::Invalid ) {
                    if( ImGui::MenuItem( localization::generic::Duplicate.Get().data() ) ) {
                        // selectedEntity = registry.DuplicateEntity(selectedEntity);
                        ImGui::CloseCurrentPopup();
                    }
                }

                ImGui::EndPopup();
            }

            ImGui::EndTable();
        }
    }
}

String EntitiesPanel::getNewEntityName() const {
    // TODO: Add logic to have unique names in scene
    String entityName = "New Entity";
    return entityName;
}

void EntitiesPanel::onDeleteAction( const input_actions::InputActionEvent& deleteAction ) {
    if( isFocused() == false )
        return;

    if( m_selectedEntity == ecs::EntityId::Invalid )
        return;

    if( deleteAction.GetData< bool >() == false )
        return;

    deleteEntity( m_selectedEntity );
}

void EntitiesPanel::deleteEntity( ecs::EntityId entity ) {
    if( entity == m_selectedEntity ) {
        setSelectedEntity( ecs::EntityId::Invalid );
    }

    SceneEditorWindow& parent = *( getParent< SceneEditorWindow >().value() );
    m_commandGraph->Push< DeleteEntityCommand >( entity,
                                                 parent.getSceneId(),
                                                 getEngineSystem< game_core::GameCoreSystem >() );
}

void EntitiesPanel::setSelectedEntity( ecs::EntityId entity ) {
    if( m_selectedEntity != entity ) {
        SceneEditorWindow& parent = *getParent< SceneEditorWindow >().value();

        ecs::EntityRegistry& registry = parent.getScene().GetRegistry();
        if( m_selectedEntity != ecs::EntityId::Invalid )
            registry.RemoveComponent< SelectedComponent >( m_selectedEntity );

        m_selectedEntity = entity;
        if( m_selectedEntity != ecs::EntityId::Invalid ) {
            registry.AddComponent< SelectedComponent >( m_selectedEntity );
        }
    }
}
} // namespace onyx::editor::scene_editor
