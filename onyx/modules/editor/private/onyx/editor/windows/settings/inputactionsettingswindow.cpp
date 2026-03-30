#include <onyx/editor/windows/settings/inputactionsettingswindow.h>

#include <onyx/input/inputid.h>
#include <onyx/input/inputsystem.h>
#include <onyx/inputactions/bindings/inputbindingsfactory.h>
#include <onyx/inputactions/inputaction.h>
#include <onyx/inputactions/inputactionsmap.h>
#include <onyx/inputactions/inputactionsserializer.h>
#include <onyx/inputactions/inputactionsystem.h>
#include <onyx/inputactions/modifiers/inputmodifier.h>
#include <onyx/inputactions/modifiers/inputmodifiersfactory.h>
#include <onyx/inputactions/triggers/inputtrigger.h>
#include <onyx/inputactions/triggers/inputtriggersfactory.h>

#include <onyx/assets/assetsystem.h>

#include <onyx/editor/windows/commandhistorywindow.h>
#include <onyx/ui/imguisystem.h>

#include <onyx/editor/commands/inputactions/addinputactioncommand.h>
#include <onyx/editor/commands/inputactions/addinputbindingcommand.h>
#include <onyx/editor/commands/inputactions/addinputmodifiercommand.h>
#include <onyx/editor/commands/inputactions/addinputtriggercommand.h>
#include <onyx/editor/commands/inputactions/bindinputbindingslotcommand.h>
#include <onyx/editor/commands/inputactions/deleteinputactioncommand.h>
#include <onyx/editor/commands/inputactions/deleteinputbindingcommand.h>
#include <onyx/editor/commands/inputactions/deleteinputmodifiercommand.h>
#include <onyx/editor/commands/inputactions/deleteinputtriggercommand.h>
#include <onyx/editor/commands/inputactions/modifyinputactioncommand.h>
#include <onyx/editor/commands/inputactions/renameinputactioncommand.h>

#include <onyx/filesystem/filedialog.h>

#include <onyx/ui/propertyinspector.h>
#include <onyx/ui/scopedcolor.h>
#include <onyx/ui/scopedid.h>
#include <onyx/ui/scopedindent.h>
#include <onyx/ui/scopedstyle.h>
#include <onyx/ui/widgets.h>

#include <onyx/editor/editor_localization.h>

#include <onyx/localization/localization.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/ui/controls/button.h>
#include <onyx/ui/controls/combobox.h>
#include <onyx/ui/propertygrid.h>

#include <imgui_internal.h>

namespace onyx::editor {
namespace {
template < typename FactoryT, typename CommandT >
bool renderCreatePopup( InputActionSettingsWindow& inputSettingsWindow,
                        ICommandGraph& commandsHistory,
                        const localization::LocalizationModule& localizationSystem ) {
    if( ImGui::BeginPopupEx( ImGui::GetItemID(),
                             ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoSavedSettings ) ) {
        // TODO: Filter bindings that are shown to the user to avoid adding unnecessarily comlex options
        const HashMap< StringId32, typename FactoryT::MetaData >& registeredBindings = FactoryT::GetTypes();
        for( auto&& [ typeId, bindingMetaData ] : registeredBindings ) {
            StringView buttonLabel = format::format( "{} {}",
                                                     localization::generic::Add,
                                                     localizationSystem.GetLocalized( typeId ) );
            if( ImGui::MenuItem( buttonLabel.data() ) ) {
                commandsHistory.Push< CommandT >( typeId, inputSettingsWindow );
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
        return true;
    }

    return false;
}

template < typename FactoryT, typename AddCommandT >
bool renderAddHeader( InputActionSettingsWindow& inputSettingsWindow,
                      ICommandGraph& commandsHistory,
                      const localization::LocalizationModule& localizationSystem ) {
    ui::ScopedImGuiStyle style{
        { ImGuiStyleVar_FrameBorderSize, 0.0f },
    };
    ui::ScopedImGuiColor color{
        { ImGuiCol_Button, 0x30000000 },
    };

    bool shouldOpen = false;
    ImGui::Spring();

    if( ui::Button( "+" ) ) {
        ImGui::OpenPopupEx( ImGui::GetItemID(), ImGuiWindowFlags_NoTitleBar );
    }

    shouldOpen = renderCreatePopup< FactoryT, AddCommandT >( inputSettingsWindow, commandsHistory, localizationSystem );

    return shouldOpen;
}
} // namespace

void InputActionSettingsWindow::onOpen() {
    setWindowFlags( ImGuiWindowFlags_MenuBar );
    ImGui::SetNextWindowSizeConstraints( ImVec2( 640, 480 ), ImVec2( FLT_MAX, FLT_MAX ) );

    input::InputSystem& inputSystem = getEngineSystem< input::InputSystem >();

    inputSystem.OnMouseAxisChange().Connect< &InputActionSettingsWindow::onMouseAxisChange >( this );
    inputSystem.OnMouseButton().Connect< &InputActionSettingsWindow::onMouseButton >( this );
    inputSystem.OnMousePositionChange().Connect< &InputActionSettingsWindow::onMousePositionChange >( this );
    inputSystem.OnKey().Connect< &InputActionSettingsWindow::onKey >( this );
    inputSystem.OnControllerAxisChange().Connect< &InputActionSettingsWindow::onControllerAxisChange >( this );
    inputSystem.OnControllerButton().Connect< &InputActionSettingsWindow::onControllerButton >( this );

    input_actions::InputActionSystem& inputActionSystem = getEngineSystem< input_actions::InputActionSystem >();
    inputActionSystem.OnInput< &InputActionSettingsWindow::onDeleteAction >( "Delete"_id64, this );

    ui::ImGuiSystem& imguiSystem = getEngineSystem< ui::ImGuiSystem >();
    CommandHistoryWindow& history = imguiSystem.OpenWindow< CommandHistoryWindow >( *this );
    // history.SetWindowClass(m_WindowClass);
    history.SetCommandQueue( m_commandsHistory );
}

void InputActionSettingsWindow::onClose() {
    input::InputSystem& inputSystem = getEngineSystem< input::InputSystem >();

    inputSystem.OnMouseAxisChange().Disconnect( this );
    inputSystem.OnMouseButton().Disconnect( this );
    inputSystem.OnMousePositionChange().Disconnect( this );
    inputSystem.OnKey().Disconnect( this );
    inputSystem.OnControllerAxisChange().Disconnect( this );
    inputSystem.OnControllerButton().Disconnect( this );
}

void InputActionSettingsWindow::onRender( ui::ImGuiSystem& /*imguiSystem*/ ) {
    //    if ( isOpen() == false )
    //        return;

    const String& assetName = "None";

    // setWindowFlags( ImGuiWindowFlags_MenuBar );

    // const char* windowName = format::format( "{}{} - [{}]###InputActionsEditor",
    //                                          m_IsDirty ? "*" : "",
    //                                          assetName.data(),
    //                                          localization::editor::InputActionSettings::Title );
    // setName( windowName );

    // if ( begin() ) {

    if( m_openInputContext.GetActions().empty() == false ) {
        HashMap< StringId32, input_actions::InputActionsMap > actionMaps{ { "t", m_openInputContext } };

        ui::ScopedImGuiStyle style( ImGuiStyleVar_CellPadding, ImVec2( 4, 4 ) );
        if( ImGui::BeginTable( "InputActions",
                               2,
                               ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp |
                                   ImGuiTableFlags_Resizable ) ) {
            ImGui::TableSetupColumn( "input_action_column", ImGuiTableColumnFlags_None, 0.4f );
            ImGui::TableSetupColumn( "input_action_data_column", ImGuiTableColumnFlags_None, 0.6f );

            ImGui::TableNextRow();

            // left panel
            ImGui::TableSetColumnIndex( 0 );
            renderInputActions();

            // right panel
            ImGui::TableSetColumnIndex( 1 );
            // ImGui::BeginVertical("###InputActionMap::InputAction::Data", ImGui::GetContentRegionAvail());

            ui::property_grid::beginPropertyGrid( "InputActionProps", 80.0f );

            if( m_selectedActionIndex != InvalidIndex32 )
                renderActionProperties();

            if( m_selectedBindingIndex != InvalidIndex32 )
                renderSelectedBindingProperties();

            ui::property_grid::endPropertyGrid();

            ImGui::EndTable();
        }
    }
    // }

    // ImGui::End();
}

void InputActionSettingsWindow::onRenderMainMenuBar() {
    assets::AssetSystem& assetSystem = getEngineSystem< assets::AssetSystem >();
    if( ImGui::BeginMenu( localization::generic::File.Get().data() ) ) {
        if( ImGui::MenuItem( localization::generic::Open.Get().data() ) ) {
            FilePath path;
            if( file_system::FileDialog::OpenFileDialog( path,
                                                         "Input actions asset",
                                                         input_actions::InputActionsSerializer::Extensions ) ) {
                assets::AssetHandle< input_actions::InputActionsContext > asset;
                assets::AssetId assetId( path );
                assetSystem.getAssetUnmanaged( assetId, asset );
                asset->getOnLoadedEvent().Connect< &InputActionSettingsWindow::onInputAssetLoaded >( this );
            }
        }

        if( ImGui::MenuItem( localization::generic::Save.Get().data() ) ) {
            // assetSystem.SaveAsset(m_EditableCopy);
        }

        if( ImGui::MenuItem( localization::generic::SaveAs.Get().data() ) ) {
            FilePath path;
            if( file_system::FileDialog::SaveFileDialog( path,
                                                         "Input actions asset",
                                                         input_actions::InputActionsSerializer::Extensions ) ) {
                // assetSystem.SaveAssetAs(path, m_EditableCopy);
            }
        }

        ImGui::EndMenu();
    }
}

void InputActionSettingsWindow::onMouseAxisChange( const input::MouseAxisEvent& /*event*/ ) {}

void InputActionSettingsWindow::onMouseButton( const input::MouseButtonEvent& event ) {
    if( m_isListeningOnInput == false )
        return;

    if( m_selectedBindingIndex == InvalidIndex32 ) {
        m_isListeningOnInput = false;
        return;
    }

    m_commandsHistory.Push< BindInputBindingSlotCommand >( m_selectedActionId,
                                                           m_selectedBindingIndex,
                                                           m_selectedBindingSlotIndex,
                                                           event.Button,
                                                           *this );
    m_isListeningOnInput = false;
}

void InputActionSettingsWindow::onMousePositionChange( const input::MousePositionEvent& /*event*/ ) {}

void InputActionSettingsWindow::onKey( const input::KeyboardEvent& event ) {
    if( m_isListeningOnInput == false )
        return;

    if( m_selectedBindingIndex == InvalidIndex32 ) {
        m_isListeningOnInput = false;
        return;
    }

    m_commandsHistory.Push< BindInputBindingSlotCommand >( m_selectedActionId,
                                                           m_selectedBindingIndex,
                                                           m_selectedBindingSlotIndex,
                                                           event.Key,
                                                           *this );
    m_isListeningOnInput = false;
}

void InputActionSettingsWindow::onControllerAxisChange( const input::GameControllerAxisEvent& event ) {
    if( m_isListeningOnInput == false )
        return;

    if( m_selectedBindingIndex == InvalidIndex32 ) {
        m_isListeningOnInput = false;
        return;
    }

    m_commandsHistory.Push< BindInputBindingSlotCommand >( m_selectedActionId,
                                                           m_selectedBindingIndex,
                                                           m_selectedBindingSlotIndex,
                                                           event.Axis,
                                                           *this );
    m_isListeningOnInput = false;
}

void InputActionSettingsWindow::onControllerButton( const input::GameControllerButtonEvent& event ) {
    if( m_isListeningOnInput == false )
        return;

    if( m_selectedBindingIndex == InvalidIndex32 ) {
        m_isListeningOnInput = false;
        return;
    }

    m_commandsHistory.Push< BindInputBindingSlotCommand >( m_selectedActionId,
                                                           m_selectedBindingIndex,
                                                           m_selectedBindingSlotIndex,
                                                           event.Button,
                                                           *this );
    m_isListeningOnInput = false;
}

void InputActionSettingsWindow::renderInputActions() {
    // actions in the selected map
    ui::ScopedImGuiId id( "###InputActionMap::InputActions" );
    // if (m_SelectedActionMapId.IsValid())
    {
        int32_t i = 0;
        input_actions::InputActionsMap& selectedMap = m_openInputContext;

        if( ImGui::BeginPopupContextItem( "###CreateInputAction", ImGuiPopupFlags_MouseButtonRight ) ) {
            if( ui::Button( localization::generic::Add ) ) {
                m_commandsHistory.Push< AddInputActionCommand >( *this );
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        for( input_actions::InputAction& action : selectedMap.GetActions() ) {
            ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_SpanAvailWidth;

            ImGuiTreeNodeFlags treeNodeFlags = baseFlags;
            bool isSelected = i == m_selectedActionIndex;
            bool wasSelected = isSelected;
            if( isSelected )
                treeNodeFlags |= ImGuiTreeNodeFlags_Selected;

            StringView itemIdString = format::format( "##{}", action.GetId().getString() );
            ImGuiID itemId = ImGui::GetID( itemIdString.data() );

            auto customHeader = [ & ]() {
                String name( action.GetId().getString() );
                if( ui::DrawRenameInput( itemIdString, name, ImVec2( -1, 0 ), isSelected ) ) {
                    m_commandsHistory.Push< RenameInputActionCommand >( action.GetId(), StringId64( name ), *this );
                }

                return renderCreatePopup< input_actions::InputBindingsFactory, AddInputBindingCommand >(
                    *this,
                    m_commandsHistory,
                    getEngineSystem< localization::LocalizationModule >() );
            };

            ui::ScopedImGuiColor color( ImGuiCol_FrameBg, 0x0 );

            bool wasOpen = ImGui::TreeNodeGetOpen( itemId );
            if( ui::ContextMenuHeader( itemIdString, customHeader, treeNodeFlags ) ) {
                ImGui::Indent();

                DynamicArray< UniquePtr< input_actions::InputBinding > >& bindings = action.GetBindings();
                renderBindings( isSelected, bindings );

                bool hasSelectedChildChanged = isSelected && ( wasSelected == false );
                if( hasSelectedChildChanged || ( wasOpen == false ) ) {
                    m_selectedActionId = action.GetId();
                    m_selectedActionIndex = i;
                    if( hasSelectedChildChanged == false ) {
                        m_selectedBindingIndex = InvalidIndex32;
                        m_selectedBindingSlotIndex = InvalidIndex32;
                    }
                }

                ImGui::Unindent();
            }

            if( ImGui::IsItemHovered() ) {
                if( ImGui::IsMouseClicked( ImGuiMouseButton_Left, ImGuiInputFlags_None ) ) {
                    m_selectedActionId = action.GetId();
                    m_selectedActionIndex = i;
                    m_selectedBindingIndex = InvalidIndex32;
                    m_selectedBindingSlotIndex = InvalidIndex32;
                }
            }

            ++i;
        }
    }
}

void InputActionSettingsWindow::renderBindings( bool& isSelected,
                                                DynamicArray< UniquePtr< input_actions::InputBinding > >& bindings ) {
    int32_t bindingsCount = static_cast< int32_t >( bindings.size() );
    for( int32_t bindingIndex = 0; bindingIndex < bindingsCount; ++bindingIndex ) {
        bool isBindingSelected = isSelected && ( m_selectedBindingIndex == bindingIndex );
        input_actions::InputBinding& binding = *bindings[ bindingIndex ];
        renderBinding( isBindingSelected, bindingIndex, binding );
    }
}

void InputActionSettingsWindow::renderBinding( bool& isSelected,
                                               int32_t bindingIndex,
                                               input_actions::InputBinding& binding ) {
    bool isBindingSelected = isSelected && ( m_selectedBindingIndex == bindingIndex );

    int32_t bindingSlotsCount = binding.GetInputBindingSlotsCount();
    const bool hasCollapsibleHeader = bindingSlotsCount > 1;
    if( hasCollapsibleHeader ) {
        ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_SpanAvailWidth;
        if( isSelected )
            treeNodeFlags |= ImGuiTreeNodeFlags_Selected;

        StringView label = format::format( "{}", binding.GetName() );
        ImGuiID headerId = ImGui::GetID( label.data() );

        bool wasOpen = ImGui::TreeNodeGetOpen( headerId );
        if( ui::ContextMenuHeader( label, treeNodeFlags ) ) {
            if( wasOpen == false ) {
                isSelected = true;
                isBindingSelected = true;
                m_selectedBindingIndex = bindingIndex;
                m_selectedBindingSlotIndex = InvalidIndex32;
            }
        } else {
            return;
        }
    }

    ui::ScopedImGuiIndent indent( hasCollapsibleHeader ? 20.0f : 4.0f );
    for( int32_t slotIndex = 0; slotIndex < bindingSlotsCount; ++slotIndex ) {
        bool isBindingSlotSelected = isBindingSelected;

        StringView boundInputLabel;
        input::InputID boundInputValue = binding.GetBoundInputForSlot( slotIndex );
        StringView bindingInputTypeLabel = input::GetInputTypeString( boundInputValue );

        if( boundInputValue == input::InputID::Invalid ) {
            boundInputLabel = localization::editor::InputActionSettings::Bindings::Unbound.Get();
        } else {
            boundInputLabel = input::ToString( boundInputValue );
        }

        StringView inputBindingSlotLabel = format::format( "{} [ {} ]", boundInputLabel, bindingInputTypeLabel );

        if( ui::Selectable( inputBindingSlotLabel, isBindingSlotSelected ) ) {
            isSelected = true;
            m_selectedBindingIndex = bindingIndex;
            m_selectedBindingSlotIndex = slotIndex;
        }
    }
}

void InputActionSettingsWindow::renderActionProperties() {
    input_actions::InputActionsMap& selectedMap = m_openInputContext;
    DynamicArray< input_actions::InputAction >& selectedActions = selectedMap.GetActions();
    input_actions::InputAction& selectedAction = selectedActions[ m_selectedActionIndex ];

    input_actions::ActionType actionType = selectedAction.GetType();

    ui::property_grid::drawProperty( "Action Id", selectedAction.GetId().getString() );

    if( ui::property_grid::drawProperty< input_actions::ActionType,
                                         input_actions::ActionType::Invalid,
                                         input_actions::ActionType::Count >( "Value Type", actionType ) ) {
        m_commandsHistory.Push< ModifyInputActionCommand >( m_selectedActionId, actionType, *this );
    }
}

void InputActionSettingsWindow::onInputAssetLoaded(
    assets::AssetHandle< input_actions::InputActionsContext > inputActionsAsset ) {
    m_isDirty = false;

    m_openInputContext = inputActionsAsset->GetMaps().begin()->second;

    m_commandsHistory.SetBase( m_openInputContext );
    m_commandsHistory.SetHead( m_openInputContext );

    m_mapsSelectedStates.clear();
    m_mapsSelectedStates.resize( 1, false );

    m_actionsSelectedStates.clear();
    m_selectedActionIndex = InvalidIndex32;
    m_selectedActionId = StringId32::Invalid;
    m_selectedBindingIndex = InvalidIndex32;
    m_selectedBindingSlotIndex = InvalidIndex32;
}

void InputActionSettingsWindow::renderSelectedBindingProperties() {
    if( m_selectedBindingIndex == InvalidIndex32 )
        return;

    Optional< input_actions::InputAction* > selectedActionOptional = m_openInputContext.GetAction( m_selectedActionId );
    ONYX_ASSERT( selectedActionOptional.has_value() );
    input_actions::InputAction& selectedAction = *selectedActionOptional.value_or( nullptr );

    DynamicArray< UniquePtr< input_actions::InputBinding > >& selectedActionBindings = selectedAction.GetBindings();
    input_actions::InputBinding& selectedBinding = *selectedActionBindings[ m_selectedBindingIndex ];

    DynamicArray< UniquePtr< input_actions::InputModifier > >& modifiers = selectedBinding.GetModifiers();
    const uint32_t modifiersCount = static_cast< uint32_t >( modifiers.size() );

    const auto addModifierFunctor = [ & ]() {
        return renderAddHeader< input_actions::InputModifiersFactory, AddInputModifierCommand >(
            *this,
            m_commandsHistory,
            getEngineSystem< localization::LocalizationModule >() );
    };

    const auto addTriggerFunctor = [ & ]() {
        return renderAddHeader< input_actions::InputTriggersFactory, AddInputTriggerCommand >(
            *this,
            m_commandsHistory,
            getEngineSystem< localization::LocalizationModule >() );
    };

    if( ui::property_grid::beginCollapsiblePropertyGroup( "Modifiers", addModifierFunctor ) ) {
        for( uint32_t i = 0; i < modifiersCount; ++i ) {
            input_actions::InputModifier& modifier = *modifiers[ i ];
            String key = format::format( "Modifier {}", i );
            String input = format::format( "{}", modifier.GetTypeId().getString() );

            const auto removeFunctor = [ &, index = i ]() {
                ImGui::Spring();

                if( ui::Button( "-" ) ) {
                    m_commandsHistory.Push< DeleteInputModifierCommand >( m_selectedActionId,
                                                                          m_selectedBindingIndex,
                                                                          index,
                                                                          *this );
                    return false;
                }

                return true;
            };

            if( ui::property_grid::beginPropertyGroup( key, removeFunctor ) ) {
                ui::property_grid::drawProperty( "##test", input );
                ui::property_grid::endPropertyGroup();
            }
        }

        ui::property_grid::endPropertyGroup();
    }

    DynamicArray< UniquePtr< input_actions::InputTrigger > >& triggers = selectedBinding.GetTriggers();
    const uint32_t triggersCount = static_cast< uint32_t >( triggers.size() );
    if( ui::property_grid::beginCollapsiblePropertyGroup( "Triggers", addTriggerFunctor ) ) {
        for( uint32_t i = 0; i < triggersCount; ++i ) {
            input_actions::InputTrigger& trigger = *triggers[ i ];
            String key = format::format( "Trigger {}", i );
            String input = format::format( "{}", trigger.GetTypeId().getString() );

            const auto removeFunctor = [ &, index = i ]() {
                ImGui::Spring();

                if( ui::Button( "-" ) ) {
                    m_commandsHistory.Push< DeleteInputTriggerCommand >( m_selectedActionId,
                                                                         m_selectedBindingIndex,
                                                                         index,
                                                                         *this );
                    return false;
                }

                return true;
            };

            if( ui::property_grid::beginPropertyGroup( key, removeFunctor ) ) {
                const input_actions::InputTriggersFactory::MetaData&
                    metaData = input_actions::InputTriggersFactory::GetMetaData( trigger.GetTypeId() );

                ui::PropertyInspectors::draw( metaData.RuntimeTypeId, &trigger, false );
                ui::property_grid::endPropertyGroup();
            }
        }

        ui::property_grid::endPropertyGroup();
    }

    if( ui::Button( localization::editor::InputActionSettings::Bindings::Listen ) ) {
        m_isListeningOnInput = true;
    }

    // ImGui::EndVertical();
}

void InputActionSettingsWindow::onDeleteAction( const input_actions::InputActionEvent& deleteAction ) {
    if( m_isListeningOnInput ) {
        return;
    }

    if( deleteAction.GetData< bool >() == false ) {
        return;
    }

    if( isFocused() == false )
        return;

    if( m_selectedBindingIndex != InvalidIndex32 ) {
        input_actions::InputActionsMap& selectedMap = m_openInputContext;
        DynamicArray< input_actions::InputAction >& availableActions = selectedMap.GetActions();
        input_actions::InputAction& selectedAction = availableActions[ m_selectedActionIndex ];

        m_commandsHistory.Push< DeleteInputBindingCommand >( selectedAction.GetId(), m_selectedBindingIndex, *this );

        m_selectedBindingIndex = InvalidIndex32;
        m_selectedBindingSlotIndex = InvalidIndex32;
    } else if( m_selectedActionIndex != InvalidIndex32 ) {
        input_actions::InputActionsMap& selectedMap = m_openInputContext;
        DynamicArray< input_actions::InputAction >& availableActions = selectedMap.GetActions();
        input_actions::InputAction& selectedAction = availableActions[ m_selectedActionIndex ];

        m_commandsHistory.Push< DeleteInputActionCommand >( selectedAction.GetId(), *this );

        m_selectedActionIndex = InvalidIndex32;
        m_selectedActionId = StringId32::Invalid;
    }
}
} // namespace onyx::editor
