#include <onyx/editor/windows/settings/inputactionsettingswindow.h>

#include <onyx/input/inputid.h>
#include <onyx/input/inputsystem.h>
#include <onyx/inputactions/inputactionsystem.h>
#include <onyx/inputactions/inputactionsmap.h>
#include <onyx/inputactions/inputactionsserializer.h>
#include <onyx/inputactions/inputaction.h>
#include <onyx/inputactions/bindings/inputbindingsfactory.h>
#include <onyx/inputactions/modifiers/inputmodifier.h>
#include <onyx/inputactions/modifiers/inputmodifiersfactory.h>
#include <onyx/inputactions/triggers/inputtrigger.h>
#include <onyx/inputactions/triggers/inputtriggersfactory.h>

#include <onyx/assets/assetsystem.h>

#include <onyx/ui/imguisystem.h>
#include <onyx/editor/windows/commandhistorywindow.h>

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

#define IMGUI_DEFINE_MATH_OPERATORS

#include <onyx/ui/scopedcolor.h>
#include <onyx/ui/scopedid.h>
#include <onyx/ui/scopedindent.h>
#include <onyx/ui/scopedstyle.h>
#include <onyx/ui/propertyinspector.h>
#include <onyx/ui/widgets.h>

#include <onyx/editor/editor_localization.h>

#include <onyx/localization/localization.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/ui/controls/button.h>
#include <onyx/ui/controls/combobox.h>

#include <imgui_internal.h>

namespace Onyx::Editor
{
    namespace
    {
        template <typename FactoryT, typename CommandT>
        bool RenderCreatePopup(InputActionSettingsWindow& inputSettingsWindow, ICommandGraph& commandsHistory, const Localization::LocalizationModule& localizationSystem)
        {
            if (ImGui::BeginPopupEx(ImGui::GetItemID(), ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings))
            {
                // TODO: Filter bindings that are shown to the user to avoid adding unnecessarily comlex options
                const HashMap<StringId32, typename FactoryT::MetaData>& registeredBindings = FactoryT::GetTypes();
                for (auto&& [typeId, bindingMetaData] : registeredBindings)
                {
                    StringView buttonLabel = Format::Format("{} {}", Localization::Generic::Add, localizationSystem.GetLocalized(typeId));
                    if (ImGui::MenuItem(buttonLabel.data()))
                    {
                        commandsHistory.Push<CommandT>(typeId, inputSettingsWindow);
                        ImGui::CloseCurrentPopup();
                    }
                }

                ImGui::EndPopup();
                return true;
            }

            return false;
        }

        template <typename FactoryT, typename AddCommandT>
        bool RenderAddHeader(InputActionSettingsWindow& inputSettingsWindow, ICommandGraph& commandsHistory, const Localization::LocalizationModule& localizationSystem)
        {
            Ui::ScopedImGuiStyle style
            {
                 { ImGuiStyleVar_FrameBorderSize, 0.0f },
            };
            Ui::ScopedImGuiColor color
            {
                { ImGuiCol_Button, 0x30000000 },
            };
        
            bool shouldOpen = false;
            ImGui::Spring();
        
            if (Ui::Button("+"))
            {
                ImGui::OpenPopupEx(ImGui::GetItemID(), ImGuiWindowFlags_NoTitleBar);
            }

            shouldOpen = RenderCreatePopup<FactoryT, AddCommandT>(inputSettingsWindow, commandsHistory, localizationSystem);
        
            return shouldOpen;
        }
    }

    void InputActionSettingsWindow::OnOpen()
    {
        Input::InputSystem& inputSystem = GetEngineSystem<Input::InputSystem>();

        inputSystem.OnMouseAxisChange().Connect<&InputActionSettingsWindow::OnMouseAxisChange>(this);
        inputSystem.OnMouseButton().Connect<&InputActionSettingsWindow::OnMouseButton>(this);
        inputSystem.OnMousePositionChange().Connect<&InputActionSettingsWindow::OnMousePositionChange>(this);
        inputSystem.OnKey().Connect<&InputActionSettingsWindow::OnKey>(this);
        inputSystem.OnControllerAxisChange().Connect<&InputActionSettingsWindow::OnControllerAxisChange>(this);
        inputSystem.OnControllerButton().Connect<&InputActionSettingsWindow::OnControllerButton>(this);

        InputActions::InputActionSystem& inputActionSystem = GetEngineSystem<InputActions::InputActionSystem>();
        inputActionSystem.OnInput<&InputActionSettingsWindow::OnDeleteAction>("Delete"_id64, this);

        Ui::ImGuiSystem& imguiSystem = GetEngineSystem<Ui::ImGuiSystem>();
        CommandHistoryWindow& history = imguiSystem.OpenWindow<CommandHistoryWindow>(*this);
        //history.SetWindowClass(m_WindowClass);
        history.SetCommandQueue(m_CommandsHistory);

    }

    void InputActionSettingsWindow::OnClose()
    {
        Input::InputSystem& inputSystem = GetEngineSystem<Input::InputSystem>();

        inputSystem.OnMouseAxisChange().Disconnect(this);
        inputSystem.OnMouseButton().Disconnect(this);
        inputSystem.OnMousePositionChange().Disconnect(this);
        inputSystem.OnKey().Disconnect(this);
        inputSystem.OnControllerAxisChange().Disconnect(this);
        inputSystem.OnControllerButton().Disconnect(this);
    }

    void InputActionSettingsWindow::OnRender(Ui::ImGuiSystem& /*imguiSystem*/)
    {
        if (IsOpen() == false)
            return;

        const String& assetName = "None";

        SetWindowFlags(ImGuiWindowFlags_MenuBar);

        ImGui::SetNextWindowSizeConstraints(ImVec2(640, 480), ImVec2(FLT_MAX, FLT_MAX));

        const char* windowName = Format::Format("{}{} - [{}]###InputActionsEditor", m_IsDirty ? "*" : "", assetName.data(), Localization::Editor::InputActionSettings::Title);
        SetName(windowName);

        if (Begin())
        {
            if (BeginMenuBar())
            {
                Assets::AssetSystem& assetSystem = GetEngineSystem<Assets::AssetSystem>();
                if (ImGui::BeginMenu(Localization::Generic::File.Get().data()))
                {
                    if (ImGui::MenuItem(Localization::Generic::Open.Get().data()))
                    {
                        FilePath path;
                        if (FileSystem::FileDialog::OpenFileDialog(path, "Input actions asset", InputActions::InputActionsSerializer::Extensions))
                        {
                            Assets::AssetHandle<InputActions::InputActionsContext> asset;
                            Assets::AssetId assetId(path);
                            assetSystem.GetAssetUnmanaged(assetId, asset);
                            asset->GetOnLoadedEvent().Connect<&InputActionSettingsWindow::OnInputAssetLoaded>(this);
                        }
                    }

                    if (ImGui::MenuItem(Localization::Generic::Save.Get().data()))
                    {
                        //assetSystem.SaveAsset(m_EditableCopy);
                    }

                    if (ImGui::MenuItem(Localization::Generic::SaveAs.Get().data()))
                    {
                        FilePath path;
                        if (FileSystem::FileDialog::SaveFileDialog(path, "Input actions asset", InputActions::InputActionsSerializer::Extensions))
                        {
                           // assetSystem.SaveAssetAs(path, m_EditableCopy);
                        }
                    }

                    ImGui::EndMenu();
                }

                EndMenuBar();
            }

            if (m_OpenInputContext.GetActions().empty() == false)
            {
                HashMap<StringId32, InputActions::InputActionsMap> actionMaps { { "t", m_OpenInputContext } };

                Ui::ScopedImGuiStyle style(ImGuiStyleVar_CellPadding, ImVec2(4, 4));
                if (ImGui::BeginTable("InputActions", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable))
                {
                    ImGui::TableSetupColumn("input_action_column", ImGuiTableColumnFlags_None, 0.4f);
                    ImGui::TableSetupColumn("input_action_data_column", ImGuiTableColumnFlags_None, 0.6f);
                    
                    ImGui::TableNextRow();
                    
                    // left panel
                    ImGui::TableSetColumnIndex(0);
                    RenderInputActions();

                    // right panel
                    ImGui::TableSetColumnIndex(1);
                    //ImGui::BeginVertical("###InputActionMap::InputAction::Data", ImGui::GetContentRegionAvail());


                    Ui::PropertyGrid::BeginPropertyGrid("InputActionProps", 80.0f);

                    if (m_SelectedActionIndex != INVALID_INDEX_32)
                        RenderActionProperties();

                    if (m_SelectedBindingIndex != INVALID_INDEX_32)
                        RenderSelectedBindingProperties();

                    Ui::PropertyGrid::EndPropertyGrid();

                    ImGui::EndTable();
                }
            }
        }

        ImGui::End();
    }

    void InputActionSettingsWindow::OnMouseAxisChange(const Input::MouseAxisEvent& /*event*/)
    {
    }

    void InputActionSettingsWindow::OnMouseButton(const Input::MouseButtonEvent& event)
    {
        if (m_IsListeningOnInput == false)
            return;

        if (m_SelectedBindingIndex == INVALID_INDEX_32)
        {
            m_IsListeningOnInput = false;
            return;
        }

        m_CommandsHistory.Push<BindInputBindingSlotCommand>(m_SelectedActionId, m_SelectedBindingIndex, m_SelectedBindingSlotIndex, event.Button, *this);
        m_IsListeningOnInput = false;
    }

    void InputActionSettingsWindow::OnMousePositionChange(const Input::MousePositionEvent& /*event*/)
    {
    }

    void InputActionSettingsWindow::OnKey(const Input::KeyboardEvent& event)
    {
        if (m_IsListeningOnInput == false)
            return;

        if (m_SelectedBindingIndex == INVALID_INDEX_32)
        {
            m_IsListeningOnInput = false;
            return;
        }

        m_CommandsHistory.Push<BindInputBindingSlotCommand>(m_SelectedActionId, m_SelectedBindingIndex, m_SelectedBindingSlotIndex, event.Key, *this);
        m_IsListeningOnInput = false;
    }

    void InputActionSettingsWindow::OnControllerAxisChange(const Input::GameControllerAxisEvent& event)
    {
        if (m_IsListeningOnInput == false)
            return;

        if (m_SelectedBindingIndex == INVALID_INDEX_32)
        {
            m_IsListeningOnInput = false;
            return;
        }

        m_CommandsHistory.Push<BindInputBindingSlotCommand>(m_SelectedActionId, m_SelectedBindingIndex, m_SelectedBindingSlotIndex, event.Axis, *this);
        m_IsListeningOnInput = false;
    }

    void InputActionSettingsWindow::OnControllerButton(const Input::GameControllerButtonEvent& event)
    {
        if (m_IsListeningOnInput == false)
            return;

        if (m_SelectedBindingIndex == INVALID_INDEX_32)
        {
            m_IsListeningOnInput = false;
            return;
        }

        m_CommandsHistory.Push<BindInputBindingSlotCommand>(m_SelectedActionId, m_SelectedBindingIndex, m_SelectedBindingSlotIndex, event.Button, *this);
        m_IsListeningOnInput = false;
    }

    void InputActionSettingsWindow::RenderInputActions()
    {
        // actions in the selected map
        Ui::ScopedImGuiId id("###InputActionMap::InputActions");
        //if (m_SelectedActionMapId.IsValid())
        {
            onyxS32 i = 0;
            InputActions::InputActionsMap& selectedMap = m_OpenInputContext;

            if (ImGui::BeginPopupContextItem("###CreateInputAction", ImGuiPopupFlags_MouseButtonRight))
            {
                if (Ui::Button(Localization::Generic::Add))
                {
                    m_CommandsHistory.Push<AddInputActionCommand>(*this);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            for (InputActions::InputAction& action : selectedMap.GetActions())
            {
                ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_SpanAvailWidth;

                ImGuiTreeNodeFlags treeNodeFlags = baseFlags;
                bool isSelected = i == m_SelectedActionIndex;
                bool wasSelected = isSelected;
                if (isSelected)
                    treeNodeFlags |= ImGuiTreeNodeFlags_Selected;

                StringView itemIdString = Format::Format("##{}", action.GetId().GetString());
                ImGuiID itemId = ImGui::GetID(itemIdString.data());

                auto customHeader = [&]()
                {
                    String name(action.GetId().GetString());
                    if (Ui::DrawRenameInput(itemIdString, name, ImVec2(-1,0), isSelected))
                    {
                        m_CommandsHistory.Push<RenameInputActionCommand>(action.GetId(), StringId64(name), *this);
                    }

                    return RenderCreatePopup<InputActions::InputBindingsFactory, AddInputBindingCommand>(*this, m_CommandsHistory, GetEngineSystem<Localization::LocalizationModule>());
                };

                Ui::ScopedImGuiColor color(ImGuiCol_FrameBg, 0x0);

                bool wasOpen = ImGui::TreeNodeGetOpen(itemId);
                if (Ui::ContextMenuHeader(itemIdString, customHeader, treeNodeFlags))
                {
                    ImGui::Indent();

                    DynamicArray<UniquePtr<InputActions::InputBinding>>& bindings = action.GetBindings();
                    RenderBindings(isSelected, bindings);

                    bool hasSelectedChildChanged = isSelected && (wasSelected == false);
                    if  (hasSelectedChildChanged  || (wasOpen == false))
                    {
                        m_SelectedActionId = action.GetId();
                        m_SelectedActionIndex = i;
                        if (hasSelectedChildChanged == false)
                        {
                            m_SelectedBindingIndex = INVALID_INDEX_32;
                            m_SelectedBindingSlotIndex = INVALID_INDEX_32;
                        }
                    }

                    
                    ImGui::Unindent();
                }

                if (ImGui::IsItemHovered())
                {
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, ImGuiInputFlags_None))
                    {
                        m_SelectedActionId = action.GetId();
                        m_SelectedActionIndex = i;
                        m_SelectedBindingIndex = INVALID_INDEX_32;
                        m_SelectedBindingSlotIndex = INVALID_INDEX_32;
                    }
                }

                ++i;
            }
        }
    }

    void InputActionSettingsWindow::RenderBindings(bool& isSelected, DynamicArray<UniquePtr<InputActions::InputBinding>>& bindings)
    {
        onyxS32 bindingsCount = static_cast<onyxS32>(bindings.size());
        for (onyxS32 bindingIndex = 0; bindingIndex < bindingsCount; ++bindingIndex)
        {
            bool isBindingSelected = isSelected && (m_SelectedBindingIndex == bindingIndex);
            InputActions::InputBinding& binding = *bindings[bindingIndex];
            RenderBinding(isBindingSelected, bindingIndex, binding);
        }
    }

    void InputActionSettingsWindow::RenderBinding(bool& isSelected, onyxS32 bindingIndex, InputActions::InputBinding& binding)
    {
        bool isBindingSelected = isSelected && (m_SelectedBindingIndex == bindingIndex);

        onyxS32 bindingSlotsCount = binding.GetInputBindingSlotsCount();
        const bool hasCollapsibleHeader = bindingSlotsCount > 1;
        if (hasCollapsibleHeader)
        {
            ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_SpanAvailWidth;
            if (isSelected)
                treeNodeFlags |= ImGuiTreeNodeFlags_Selected;

            StringView label = Format::Format("{}", binding.GetName());
            ImGuiID headerId = ImGui::GetID(label.data());

            bool wasOpen = ImGui::TreeNodeGetOpen(headerId);
            if (Ui::ContextMenuHeader(label, treeNodeFlags))
            {
                if (wasOpen == false)
                {
                    isSelected = true;
                    isBindingSelected = true;
                    m_SelectedBindingIndex = bindingIndex;
                    m_SelectedBindingSlotIndex = ONYX_INVALID_INDEX;
                }
            }
            else
            {
                return;
            }
        }

        Ui::ScopedImGuiIndent indent(hasCollapsibleHeader ? 20.0f : 4.0f);
        for (onyxS32 slotIndex = 0; slotIndex < bindingSlotsCount; ++slotIndex)
        {
            bool isBindingSlotSelected = isBindingSelected;

            StringView boundInputLabel;
            Input::InputID boundInputValue = binding.GetBoundInputForSlot(slotIndex);
            StringView bindingInputTypeLabel = Input::GetInputTypeString(boundInputValue);
            
            if (boundInputValue == Input::InputID::Invalid)
            {
                boundInputLabel = Localization::Editor::InputActionSettings::Bindings::Unbound.Get();
            }
            else
            {
                boundInputLabel = Input::ToString(boundInputValue);
            }

            StringView inputBindingSlotLabel = Format::Format("{} [ {} ]", boundInputLabel, bindingInputTypeLabel);
            
            if (Ui::Selectable(inputBindingSlotLabel, isBindingSlotSelected))
            {
                isSelected = true;
                m_SelectedBindingIndex = bindingIndex;
                m_SelectedBindingSlotIndex = slotIndex;
            }
        }
    }

    void InputActionSettingsWindow::RenderActionProperties()
    {
        InputActions::InputActionsMap& selectedMap = m_OpenInputContext;
        DynamicArray<InputActions::InputAction>& selectedActions = selectedMap.GetActions();
        InputActions::InputAction& selectedAction = selectedActions[m_SelectedActionIndex];

        InputActions::ActionType actionType = selectedAction.GetType();

        Ui::PropertyGrid::DrawProperty("Action Id", selectedAction.GetId().GetString());

        if (Ui::PropertyGrid::DrawProperty<InputActions::ActionType, InputActions::ActionType::Invalid, InputActions::ActionType::Count>("Value Type", actionType))
        {
            m_CommandsHistory.Push<ModifyInputActionCommand>(m_SelectedActionId, actionType, *this);
        }
    }

    void InputActionSettingsWindow::OnInputAssetLoaded(Assets::AssetHandle<InputActions::InputActionsContext> inputActionsAsset)
    {
        m_IsDirty = false;

        m_OpenInputContext = inputActionsAsset->GetMaps().begin()->second;

        m_CommandsHistory.SetBase(m_OpenInputContext);
        m_CommandsHistory.SetHead(m_OpenInputContext);

        m_MapsSelectedStates.clear();
        m_MapsSelectedStates.resize(1, false);

        m_ActionsSelectedStates.clear();
        m_SelectedActionIndex = INVALID_INDEX_32;
        m_SelectedActionId = StringId32::Invalid;
        m_SelectedBindingIndex = INVALID_INDEX_32;
        m_SelectedBindingSlotIndex = INVALID_INDEX_32;
    }

    void InputActionSettingsWindow::RenderSelectedBindingProperties()
    {
        if (m_SelectedBindingIndex == INVALID_INDEX_32)
            return;

        Optional<InputActions::InputAction*> selectedActionOptional = m_OpenInputContext.GetAction(m_SelectedActionId);
        ONYX_ASSERT(selectedActionOptional.has_value());
        InputActions::InputAction& selectedAction = *selectedActionOptional.value_or(nullptr);

        DynamicArray<UniquePtr<InputActions::InputBinding>>& selectedActionBindings = selectedAction.GetBindings();
        InputActions::InputBinding& selectedBinding = *selectedActionBindings[m_SelectedBindingIndex];

        DynamicArray<UniquePtr<InputActions::InputModifier>>& modifiers = selectedBinding.GetModifiers();
        const onyxU32 modifiersCount = static_cast<onyxU32>(modifiers.size());
        
        const auto AddModifierFunctor = [&]()
        {
            return RenderAddHeader<InputActions::InputModifiersFactory, AddInputModifierCommand>(*this, m_CommandsHistory, GetEngineSystem<Localization::LocalizationModule>());
        };
        
        const auto AddTriggerFunctor = [&]()
        {
            return RenderAddHeader<InputActions::InputTriggersFactory, AddInputTriggerCommand>(*this, m_CommandsHistory, GetEngineSystem<Localization::LocalizationModule>());
        };

        if (Ui::PropertyGrid::BeginCollapsiblePropertyGroup("Modifiers", AddModifierFunctor))
        {
            for (onyxU32 i = 0; i < modifiersCount; ++i)
            {
                InputActions::InputModifier& modifier = *modifiers[i];
                String key = Format::Format("Modifier {}", i);
                String input = Format::Format("{}", modifier.GetTypeId().GetString());

                const auto RemoveFunctor = [&, index = i]()
                {
                    ImGui::Spring();
                
                    if (Ui::Button("-"))
                    {
                        m_CommandsHistory.Push<DeleteInputModifierCommand>(m_SelectedActionId, m_SelectedBindingIndex, index, *this);
                        return false;
                    }

                    return true;
                };

                if (Ui::PropertyGrid::BeginPropertyGroup(key, RemoveFunctor))
                {
                    Ui::PropertyGrid::DrawProperty("##test", input);
                    Ui::PropertyGrid::EndPropertyGroup();
                }
            }
    
            Ui::PropertyGrid::EndPropertyGroup();
        }

        DynamicArray<UniquePtr<InputActions::InputTrigger>>& triggers = selectedBinding.GetTriggers();
        const onyxU32 triggersCount = static_cast<onyxU32>(triggers.size());
        if (Ui::PropertyGrid::BeginCollapsiblePropertyGroup("Triggers", AddTriggerFunctor))
        {
            for (onyxU32 i = 0; i < triggersCount; ++i)
            {
                InputActions::InputTrigger& trigger = *triggers[i];
                String key = Format::Format("Trigger {}", i);
                String input = Format::Format("{}", trigger.GetTypeId().GetString());

                const auto RemoveFunctor = [&, index = i]()
                {
                    ImGui::Spring();
                
                    if (Ui::Button("-"))
                    {
                        m_CommandsHistory.Push<DeleteInputTriggerCommand>(m_SelectedActionId, m_SelectedBindingIndex, index, *this);
                        return false;
                    }

                    return true;
                };

                if( Ui::PropertyGrid::BeginPropertyGroup(key, RemoveFunctor) )
                {
                    const InputActions::InputTriggersFactory::MetaData& metaData = InputActions::InputTriggersFactory::GetMetaData(trigger.GetTypeId());

                    Ui::PropertyInspectors::Draw(metaData.RuntimeTypeId, &trigger, false);
                    Ui::PropertyGrid::EndPropertyGroup();
                }                
            }
    
            Ui::PropertyGrid::EndPropertyGroup();
        }

        if (Ui::Button(Localization::Editor::InputActionSettings::Bindings::Listen))
        {
            m_IsListeningOnInput = true;
        }
        
        //ImGui::EndVertical();
    }

    void InputActionSettingsWindow::OnDeleteAction(const InputActions::InputActionEvent& deleteAction)
    {
        if ( m_IsListeningOnInput )
        {
            return;
        }

        if (deleteAction.GetData<bool>() == false)
        {
            return;
        }

        if (IsFocused() == false)
            return;

        if( m_SelectedBindingIndex != INVALID_INDEX_32 )
        {
            InputActions::InputActionsMap& selectedMap = m_OpenInputContext;
            DynamicArray<InputActions::InputAction>& availableActions = selectedMap.GetActions();
            InputActions::InputAction& selectedAction = availableActions[m_SelectedActionIndex];

            m_CommandsHistory.Push<DeleteInputBindingCommand>(selectedAction.GetId(), m_SelectedBindingIndex, *this);
            
            m_SelectedBindingIndex = INVALID_INDEX_32;
            m_SelectedBindingSlotIndex = INVALID_INDEX_32;
        }
        else if( m_SelectedActionIndex != INVALID_INDEX_32 )
        {
            InputActions::InputActionsMap& selectedMap = m_OpenInputContext;
            DynamicArray<InputActions::InputAction>& availableActions = selectedMap.GetActions();
            InputActions::InputAction& selectedAction = availableActions[m_SelectedActionIndex];

            m_CommandsHistory.Push<DeleteInputActionCommand>(selectedAction.GetId(), *this);
            
            m_SelectedActionIndex = INVALID_INDEX_32;
            m_SelectedActionId = StringId32::Invalid;
        }
        
    }
}
