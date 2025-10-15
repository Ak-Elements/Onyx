#include <editor/windows/settings/inputactionsettingswindow.h>

#include <onyx/input/inputactionsmap.h>
#include <onyx/input/inputid.h>
#include <onyx/input/inputsystem.h>
#include <onyx/input/inputactionsserializer.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/input/inputaction.h>

#include <onyx/filesystem/filedialog.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <onyx/ui/scopedcolor.h>
#include <onyx/ui/scopedid.h>
#include <onyx/ui/scopedindent.h>
#include <onyx/ui/scopedstyle.h>
#include <onyx/ui/widgets.h>

#include <editor/editor_localization.h>

#include <onyx/input/bindings/inputbindingsfactory.h>
#include <onyx/localization/localization.h>
#include <onyx/localization/localizationmodule.h>
#include <onyx/ui/propertygrid.h>
#include <onyx/ui/controls/button.h>
#include <onyx/ui/controls/combobox.h>

#include <imgui_internal.h>

namespace Onyx::Editor
{
    InputActionSettingsWindow::InputActionSettingsWindow(Assets::AssetSystem& assetSystem, Localization::LocalizationModule& localizationModule, Input::InputSystem& inputSystem)
        : m_AssetSystem(&assetSystem)
        , m_LocalizationModule(&localizationModule)
    {
        inputSystem.AddOnInputHandler(this, &InputActionSettingsWindow::OnInputEvent);
    }

    InputActionSettingsWindow::~InputActionSettingsWindow()
    {
    }

    void InputActionSettingsWindow::OnRender(Ui::ImGuiSystem& /*imguiSystem*/)
    {
        if (IsOpen() == false)
            return;

        const String& assetName = m_EditableCopy.IsValid() ? m_EditableCopy->GetName() : "None";

        SetWindowFlags(ImGuiWindowFlags_MenuBar);

        ImGui::SetNextWindowSizeConstraints(ImVec2(640, 480), ImVec2(FLT_MAX, FLT_MAX));

        const char* windowName = Format::Format("{}{} - [{}]###InputActionsEditor", m_IsDirty ? "*" : "", assetName.data(), Localization::Editor::InputActionSettings::Title);
        SetName(windowName);

        if (Begin())
        {
            if (BeginMenuBar())
            {
                if (ImGui::BeginMenu(Localization::Generic::File.Get().data()))
                {
                    if (ImGui::MenuItem(Localization::Generic::Open.Get().data()))
                    {
                        FileSystem::Filepath path;
                        if (FileSystem::FileDialog::OpenFileDialog(path, "Input actions asset", Input::InputActionsSerializer::Extensions))
                        {
                            Assets::AssetId assetId(path);
                            m_AssetSystem->GetAssetUnmanaged(assetId, m_EditableCopy);
                            m_EditableCopy->GetOnLoadedEvent().Connect<&InputActionSettingsWindow::OnInputAssetLoaded>(this);
                        }
                    }

                    if (ImGui::MenuItem(Localization::Generic::Save.Get().data()))
                    {
                        m_AssetSystem->SaveAsset(m_EditableCopy);
                    }

                    if (ImGui::MenuItem(Localization::Generic::SaveAs.Get().data()))
                    {
                        FileSystem::Filepath path;
                        if (FileSystem::FileDialog::SaveFileDialog(path, "Input actions asset", Input::InputActionsSerializer::Extensions))
                        {
                            m_AssetSystem->SaveAssetAs(path, m_EditableCopy);
                        }
                    }

                    ImGui::EndMenu();
                }

                EndMenuBar();
            }

            if (m_EditableCopy.IsValid() && m_EditableCopy->IsLoaded())
            {
                HashMap<StringId32, Input::InputActionsMap>& actionMaps = m_EditableCopy->GetMaps();

                Ui::ScopedImGuiStyle style(ImGuiStyleVar_CellPadding, ImVec2(4, 4));
                if (ImGui::BeginTable("InputActions", 3, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable))
                {
                    ImGui::TableSetupColumn("input_action_map_column", 0, 0.2f);
                    ImGui::TableSetupColumn("input_action_column", ImGuiTableColumnFlags_None, 0.4f);
                    ImGui::TableSetupColumn("input_action_data_column", ImGuiTableColumnFlags_None, 0.4f);
                    
                    ImGui::TableNextRow();
                    
                    // left panel
                    ImGui::TableSetColumnIndex(0);
                    RenderActionMaps(actionMaps);

                    // middle panel
                    ImGui::TableSetColumnIndex(1);
                    RenderInputActions();

                    // right panel
                    ImGui::TableSetColumnIndex(2);
                    //ImGui::BeginVertical("###InputActionMap::InputAction::Data", ImGui::GetContentRegionAvail());


                    Ui::PropertyGrid::BeginPropertyGrid("InputActionProps", 80.0f);

                    if (m_SelectedActionMapId.IsValid() && (m_SelectedActionIndex != INVALID_INDEX_32))
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

    void InputActionSettingsWindow::OnInputEvent(const Input::InputEvent* inputEvent)
    {
        if (m_IsListeningOnInput == false)
            return;

        if (m_SelectedBindingIndex == INVALID_INDEX_32)
        {
            m_IsListeningOnInput = false;
            return;
        }

        if (inputEvent->m_Id == Input::InputEventType::MousePositionChanged)
            return;

        Input::InputActionsMap& selectedMap = m_EditableCopy->GetContext(m_SelectedActionMapId);
        DynamicArray<Input::InputAction>& availableActions = selectedMap.GetActions();
        Input::InputAction& selectedAction = availableActions[m_SelectedActionIndex];
        DynamicArray<UniquePtr<Input::InputBinding>>& selectedActionBindings = selectedAction.GetBindings();
        Input::InputBinding& selectedInputBinding = *selectedActionBindings[m_SelectedBindingIndex];

        if (inputEvent->IsMouseEvent())
        {
            const Input::MouseEvent* mouseEvent = static_cast<const Input::MouseEvent*>(inputEvent);
            selectedInputBinding.SetInputBindingSlot(m_SelectedBindingSlotIndex, Input::InputID{ .ID = Enums::ToIntegral(mouseEvent->m_Button) });
        }
        else if (inputEvent->IsKeyboardEvent())
        {
            const Input::KeyboardEvent* keyboardEvent = static_cast<const Input::KeyboardEvent*>(inputEvent);
            selectedInputBinding.SetInputBindingSlot(m_SelectedBindingSlotIndex, Input::InputID{ .ID = Enums::ToIntegral(keyboardEvent->m_Key) });
            }
        else if (inputEvent->IsGamepadButtonEvent())
        {
            const Input::GameControllerButtonEvent* gamepadButtonEvent = static_cast<const Input::GameControllerButtonEvent*>(inputEvent);
            selectedInputBinding.SetInputBindingSlot(m_SelectedBindingSlotIndex, Input::InputID{ .ID = Enums::ToIntegral(gamepadButtonEvent->m_Button) });
            }
        else if (inputEvent->IsGamepadAxisEvent())
        {
            const Input::GameControllerAxisEvent* gamepadAxisEvent = static_cast<const Input::GameControllerAxisEvent*>(inputEvent);
            selectedInputBinding.SetInputBindingSlot(m_SelectedBindingSlotIndex, Input::InputID{ .ID = Enums::ToIntegral(gamepadAxisEvent->m_Axis) });
        }
        
        m_IsListeningOnInput = false;
    }

    void InputActionSettingsWindow::RenderActionMaps(HashMap<StringId32, Input::InputActionsMap>& actionMaps)
    {
        // all action maps of the project
        ImGui::BeginListBox("###InputActionMaps", ImGui::GetContentRegionAvail());

        onyxU32 i = 0;
        for (auto&& [key, actionMap] : actionMaps)
        {
            if (Ui::Selectable(actionMap.GetName(), m_MapsSelectedStates[i++], false, ImGuiSelectableFlags_SelectOnClick | ImGuiSelectableFlags_SpanAvailWidth))
            {
                if (m_SelectedActionMapId != key)
                {
                    m_SelectedActionMapId = key;

                    m_ActionsSelectedStates.clear();
                    m_ActionsSelectedStates.resize(m_EditableCopy->GetContext(m_SelectedActionMapId).GetActions().size());
                }
            }
        }

        ImGui::EndListBox();
    }

    void InputActionSettingsWindow::RenderInputActions()
    {
        // actions in the selected map
        Ui::ScopedImGuiId id("###InputActionMap::InputActions");
        if (m_SelectedActionMapId.IsValid())
        {
            onyxS32 i = 0;
            Input::InputActionsMap& selectedMap = m_EditableCopy->GetContext(m_SelectedActionMapId);

            if (ImGui::BeginPopupContextItem("###CreateInputAction", ImGuiPopupFlags_MouseButtonRight))
            {
                if (Ui::Button(Localization::Generic::Add))
                {
                    Input::InputAction& newAction = selectedMap.GetActions().emplace_back("New Action");
                    newAction.SetType(Input::ActionType::Bool);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            for (Input::InputAction& action : selectedMap.GetActions())
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
                        action.SetName(name);
                    }

                    if (ImGui::BeginPopupEx(ImGui::GetItemID(), ImGuiWindowFlags_None))
                    {
                        //const HashMap<StringId32, InplaceFunction<UniquePtr<Input::InputBinding>()>>& registeredBindings = Input::InputBindingsRegistry::GetRegisteredBindings();
                        //for (auto&& [typeId, createFunctor] : registeredBindings)
                        //{
                        //    StringView buttonLabel = Format::Format("{} {}", Localization::Generic::Add, m_LocalizationModule->GetLocalized(typeId));
                        //    if (ImGui::MenuItem(buttonLabel.data()))
                        //    {
                        //        action.GetBindings().emplace_back(createFunctor());
                        //        ImGui::CloseCurrentPopup();
                        //    }
                        //}

                        ImGui::EndPopup();

                        return true;
                    }

                    return false;
                };

                Ui::ScopedImGuiColor color(ImGuiCol_FrameBg, 0x0);

                bool wasOpen = ImGui::TreeNodeGetOpen(itemId);
                if (Ui::ContextMenuHeader(itemIdString, customHeader, treeNodeFlags))
                {
                    ImGui::Indent();

                    DynamicArray<UniquePtr<Input::InputBinding>>& bindings = action.GetBindings();
                    RenderBindings(isSelected, bindings);

                    bool hasSelectedChildChanged = isSelected && (wasSelected == false);
                    if  (hasSelectedChildChanged  || (wasOpen == false))
                    {
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
                        m_SelectedActionIndex = i;
                        m_SelectedBindingIndex = INVALID_INDEX_32;
                        m_SelectedBindingSlotIndex = INVALID_INDEX_32;
                    }
                }

                ++i;
            }
        }
    }

    void InputActionSettingsWindow::RenderBindings(bool& isSelected, DynamicArray<UniquePtr<Input::InputBinding>>& bindings)
    {
        onyxS32 bindingsCount = static_cast<onyxS32>(bindings.size());
        for (onyxS32 bindingIndex = 0; bindingIndex < bindingsCount; ++bindingIndex)
        {
            bool isBindingSelected = isSelected && (m_SelectedBindingIndex == bindingIndex);
            Input::InputBinding& binding = *bindings[bindingIndex];
            RenderBinding(isBindingSelected, bindingIndex, binding);
        }
    }

    void InputActionSettingsWindow::RenderBinding(bool& isSelected, onyxS32 bindingIndex, Input::InputBinding& binding)
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
            StringView bindingInputTypeLabel = Enums::ToString(Input::GetInputType(boundInputValue));

            if (boundInputValue == Input::InputID::Invalid)
            {
                boundInputLabel = Localization::Editor::InputActionSettings::Bindings::Unbound.Get();
            }
            else
            {
                boundInputLabel = Input::ToString(boundInputValue);
            }

            StringView inputBindingSlotLabel = Format::Format("{} [{}]", boundInputLabel, bindingInputTypeLabel);
            
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
        Input::InputActionsMap& selectedMap = m_EditableCopy->GetContext(m_SelectedActionMapId);
        DynamicArray<Input::InputAction>& selectedActions = selectedMap.GetActions();
        Input::InputAction& selectedAction = selectedActions[m_SelectedActionIndex];

        Input::ActionType actionType = selectedAction.GetType();

        Ui::PropertyGrid::DrawProperty("Action Id", selectedAction.GetId().GetString());


        //Localization::LocalizationId localizationId{ Enums::ToString(actionType) };
        //Localization::LocalizedString selectedActionLocalized = m_LocalizationModule->GetLocalized(localizationId);
        if (Ui::PropertyGrid::DrawEnumProperty<Input::ActionType, Input::ActionType::Invalid, Input::ActionType::Count>("Value Type", actionType))
        {
            selectedAction.GetBindings().clear();
            selectedAction.SetType(actionType);
        }
    }


    void InputActionSettingsWindow::OnInputAssetLoaded(Reference<Input::InputActionsAsset>& inputActionsAsset)
    {
        m_IsDirty = false;
        //m_CurrentActionMapAsset = &actionMapAsset;

        m_EditableCopy = inputActionsAsset;

        m_MapsSelectedStates.clear();
        m_MapsSelectedStates.resize(m_EditableCopy->GetMaps().size(), false);

        m_ActionsSelectedStates.clear();
        m_SelectedActionMapId = 0;
        m_SelectedActionIndex = INVALID_INDEX_32;
        m_SelectedBindingIndex = INVALID_INDEX_32;
        m_SelectedBindingSlotIndex = INVALID_INDEX_32;
    }

    void InputActionSettingsWindow::RenderSelectedBindingProperties()
    {
        if (m_SelectedBindingIndex == INVALID_INDEX_32)
            return;

        //Input::InputActionsMap& selectedMap = m_EditableCopy->GetContext(m_SelectedActionMapId);
        //DynamicArray<Input::InputAction>& availableActions = selectedMap.GetActions();
        //Input::InputAction& selectedAction = availableActions[m_SelectedActionIndex];
        //DynamicArray<Input::InputBinding>& selectedActionBindings = selectedAction.GetBindings();
        //Input::InputBinding& selectedInputBinding = selectedActionBindings[m_SelectedBindingIndex];

        //ImGui::BeginVertical("props");


        //DynamicArray<Input::InputModifier>& modifiers = selectedInputBinding->GetModifiers();
        //const onyxU32 modifiersCount = static_cast<onyxU32>(modifiers.size());
        //
        //auto AddModifierFunctor = [&]()
        //{
        //    Ui::ScopedImGuiStyle style
        //    {
        //         { ImGuiStyleVar_FrameBorderSize, 0.0f },
        //    };
        //    Ui::ScopedImGuiColor color
        //    {
        //        { ImGuiCol_Button, 0x30000000 },
        //    };
        //
        //    bool shouldOpen = false;
        //    ImGui::Spring();
        //
        //    if (Ui::Button("+"))
        //    {
        //        modifiers.emplace_back(0, selectedInputBinding->GetInputType());
        //        shouldOpen = true;
        //    }
        //
        //    return shouldOpen;
        //};
        //
        //if (Ui::PropertyGrid::BeginCollapsiblePropertyGroup("Modifiers", AddModifierFunctor))
        //{
        //    for (onyxU32 i = 0; i < modifiersCount; ++i)
        //    {
        //        Input::InputModifier& modifier = modifiers[i];
        //        String input = Format::Format("{}", modifier.Input);
        //        Ui::PropertyGrid::DrawPropertyName("Input");
        //        Ui::PropertyGrid::DrawPropertyValue([&]()
        //        {
        //            ImGui::Text(input.data());
        //            if (Ui::Button(Localization::Editor::InputActionSettings::Bindings::Listen))
        //            {
        //                m_IsListeningOnInput = true;
        //            }
        //        } );
        //    }
        //
        //    Ui::PropertyGrid::EndPropertyGroup();
        //}
        //

        if (Ui::Button(Localization::Editor::InputActionSettings::Bindings::Listen))
        {
            m_IsListeningOnInput = true;
        }
        
        //ImGui::EndVertical();
    }
}
