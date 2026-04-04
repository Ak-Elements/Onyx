#include <onyx/editor/panels/sceneeditor/componentspanel.h>

#include <onyx/editor/commands/commandgraph.h>
#include <onyx/editor/commands/scene/addcomponentcommand.h>
#include <onyx/editor/commands/scene/modifycomponentcommand.h>
#include <onyx/editor/editor_localization.h>
#include <onyx/editor/windows/sceneeditor.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>

#include <onyx/entity/componentfactory.h>
#include <onyx/entity/entity.h>
#include <onyx/entity/entityregistry.h>

#include <onyx/localization/localizationmodule.h>
#include <onyx/localization/localizedstring.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/ui/scopedid.h>

#include <imgui_internal.h>
#include <onyx/localization/localization.h>
#include <onyx/ui/controls/treeview.h>
#include <onyx/ui/propertyinspector.h>

namespace onyx::editor::scene_editor {
void ComponentsPanel::onOpen() {}

void ComponentsPanel::onClose() {}

void ComponentsPanel::onRender( ui::ImGuiSystem& /*imguiSystem*/ ) {
    // setName( String( getWindowId() ) );
    //  begin();

    ImGui::Checkbox( localization::editor::ComponentsPanel::ShowAll.Get().data(), &m_ShowAll );

    game_core::GameCoreSystem& gameCoreSystem = getEngineSystem< game_core::GameCoreSystem >();
    const localization::LocalizationModule& localizationModule = getEngineSystem< localization::LocalizationModule >();

    SceneEditorWindow& parent = *getParent< SceneEditorWindow >().value();
    game_core::Scene& scene = parent.getScene();

    ecs::EntityRegistry& registry = scene.GetRegistry();

    DrawSelectedEntityComponents( registry, parent.getSceneId(), gameCoreSystem, localizationModule );

    auto selectedEntities = registry.GetView< SelectedComponent >();
    if( selectedEntities.empty() == false ) {
        DrawCreateComponentContextMenu( registry, parent.getSceneId(), gameCoreSystem, localizationModule );
    }

    // end();
}

void ComponentsPanel::DrawSelectedEntityComponents( ecs::EntityRegistry& registry,
                                                    assets::AssetId sceneId,
                                                    game_core::GameCoreSystem& gameCoreSystem,
                                                    const localization::LocalizationModule& localizationModule ) {
    const ecs::ComponentFactory& componentFactory = gameCoreSystem.GetComponentFactory();
    auto selectedEntities = registry.GetView< SelectedComponent >();

    for( ecs::EntityId selectedEntity : selectedEntities ) {
        for( auto&& componentStorageIt : registry.GetStorage() ) {
            // if the component storage contains the entity we know that the entity has this component
            if( entt::basic_sparse_set< ecs::EntityId >& componentStorage = componentStorageIt.second;
                componentStorage.contains( selectedEntity ) ) {
                if( const ecs::IComponentMeta* componentMeta = componentFactory
                                                                   .GetComponentMeta( componentStorageIt.first )
                                                                   .value_or( nullptr ) ) {
                    if( ui::PropertyInspectors::isTypeRegistered( componentMeta->getRuntimeTypeId() ) == false ) {
                        continue;
                    }

                    const StringId32 componentTypeId = componentMeta->getTypeId();
                    ui::ScopedImGuiId id( static_cast< int32_t >( componentTypeId.getId() ) );
                    ui::ScopedImGuiStyle style{ { ImGuiStyleVar_FrameBorderSize, 0.0f },
                                                { ImGuiStyleVar_ItemSpacing, ImVec2( 0.0, 0.0f ) },
                                                { ImGuiStyleVar_ItemInnerSpacing, ImVec2( 0.0, 0.0f ) } };

                    onyx::localization::LocalizationId localizationId( componentTypeId );
                    localization::LocalizedString componentName = localizationModule.GetLocalized( localizationId );
                    if( ui::ContextMenuHeader( componentName,
                                               ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_DefaultOpen ) ) {
                        ImGui::BeginChild( "Panel",
                                           ImVec2( 0, 0 ),
                                           ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY );

                        // manually clear style
                        style.Reset();

                        ui::property_grid::beginPropertyGrid( "Properties", 180.0f );

                        void* componentPtr = componentStorage.value( selectedEntity );

                        bool hasModified = ui::PropertyInspectors::draw( componentMeta->getRuntimeTypeId(),
                                                                         componentPtr,
                                                                         m_ShowAll );

                        if( hasModified ) {
                            std::any component;
                            componentMeta->copy( componentPtr, component );
                            m_CommandGraph->Push< ModifyComponentCommand >( selectedEntity,
                                                                            componentTypeId,
                                                                            std::move( component ),
                                                                            sceneId,
                                                                            gameCoreSystem );
                        }

                        // TODO: Needed for now to not auto extend if component is empty
                        ImGui::Dummy( ImVec2( 1, 1 ) );

                        ui::property_grid::endPropertyGrid();
                        ImGui::EndChild();
                    } else {
                        style.Reset();
                        ImGui::Spacing();
                    }
                }
            }
        }
    }
}

void ComponentsPanel::DrawCreateComponentContextMenu( ecs::EntityRegistry& registry,
                                                      assets::AssetId sceneId,
                                                      game_core::GameCoreSystem& gameCoreSystem,
                                                      const localization::LocalizationModule& localizationModule ) {
    if( ImGui::BeginPopupContextWindow( "AddComponentModal", ImGuiPopupFlags_MouseButtonRight ) ) {
        static String s_SearchString;
        static bool s_HasFocus = false;

        const bool isAppearing = ImGui::IsWindowAppearing();
        s_HasFocus |= isAppearing;

        if( isAppearing ) {
            s_SearchString.clear();
        }

        ui::DrawSearchBar( s_SearchString, localization::generic::Search.Get(), s_HasFocus );

        if( ImGui::BeginChild( "##ScrollList", ImVec2( 350.0f, 350.0f ) ) ) {
            ui::TreeViewFlags flags = isAppearing ? ui::TreeViewFlags::ForceCloseAll
                                                  : ( s_SearchString.empty() ? ui::TreeViewFlags::None
                                                                             : ui::TreeViewFlags::ForceOpenAll );
            ui::TreeItem root = BuildComponentTree( s_SearchString,
                                                    registry,
                                                    sceneId,
                                                    gameCoreSystem,
                                                    localizationModule );
            ui::RenderTreeView( "componentMenu", root, flags );
        }

        ImGui::EndChild();

        ImGui::EndPopup();
    }
}

ui::TreeItem ComponentsPanel::BuildComponentTree( StringView searchString,
                                                  ecs::EntityRegistry& registry,
                                                  assets::AssetId sceneId,
                                                  game_core::GameCoreSystem& gameCoreSystem,
                                                  const localization::LocalizationModule& localizationModule ) const {
    ui::TreeItem root;
    const ecs::ComponentFactory& componentFactory = gameCoreSystem.GetComponentFactory();
    for( auto&& [ componentTypeId, componentMeta ] : componentFactory.GetComponentMeta() ) {
        if( componentMeta->isTransient() || ( componentMeta->isCodeOnly() ) ) {
            continue;
        }

        localization::LocalizationId localizationId( componentTypeId );
        localization::LocalizedString localizedString = localizationModule.GetLocalized( localizationId );
        StringView componentName = localizedString.Get();
        if( ignoreCaseFind( componentName, searchString ) == StringView::npos ) {
            continue;
        }

        constexpr char delimiter = '/';
        DynamicArray< String > parts = split( componentName, delimiter );

        ui::TreeItem* currentParent = &root;
        for( uint32_t i = 0; i < parts.size(); ++i ) {
            const String& currentToken = parts[ i ];
            if( i == parts.size() - 1 ) {
                ui::TreeItem& menuItem = currentParent->Children[ currentToken ];
                menuItem.Label = currentToken;
                menuItem.OnSelected = [ & ]() {
                    auto selectedEntities = registry.GetView< SelectedComponent >();
                    auto size = selectedEntities.size();
                    ONYX_UNUSED( size );
                    for( ecs::EntityId selectedEntity : selectedEntities ) {
                        m_CommandGraph->Push< AddComponentCommand >( selectedEntity,
                                                                     componentTypeId,
                                                                     sceneId,
                                                                     gameCoreSystem );
                    }

                    ImGui::CloseCurrentPopup();
                };
                break;
            }

            currentParent = &currentParent->Children[ currentToken ];
            currentParent->Label = currentToken;
        }
    }

    return root;
}
} // namespace onyx::editor::scene_editor
