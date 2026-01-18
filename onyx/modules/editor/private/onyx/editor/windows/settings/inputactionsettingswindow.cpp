#include <onyx/editor/windows/settings/inputactionsettingswindow.h>

#include <onyx/input/inputid.h>
#include <onyx/input/inputsystem.h>
#include <onyx/inputactions/inputactionsmap.h>
#include <onyx/inputactions/inputactionsserializer.h>
#include <onyx/inputactions/inputaction.h>
#include <onyx/inputactions/bindings/inputbindingsfactory.h>
#include <onyx/assets/assetsystem.h>

#include <onyx/filesystem/filedialog.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <onyx/ui/scopedcolor.h>
#include <onyx/ui/scopedid.h>
#include <onyx/ui/scopedindent.h>
#include <onyx/ui/scopedstyle.h>
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
    InputActionSettingsWindow::InputActionSettingsWindow(Assets::AssetSystem& assetSystem, Input::InputSystem& inputSystem)
        : m_AssetSystem(&assetSystem)
        , m_InputSystem(&inputSystem)
    {
        inputSystem.OnMouseAxisChange().Connect<&InputActionSettingsWindow::OnMouseAxisChange>(this);
        inputSystem.OnMouseButton().Connect<&InputActionSettingsWindow::OnMouseButton>(this);
        inputSystem.OnMousePositionChange().Connect<&InputActionSettingsWindow::OnMousePositionChange>(this);
        inputSystem.OnKey().Connect<&InputActionSettingsWindow::OnKey>(this);
        inputSystem.OnControllerAxisChange().Connect<&InputActionSettingsWindow::OnControllerAxisChange>(this);
        inputSystem.OnControllerButton().Connect<&InputActionSettingsWindow::OnControllerButton>(this);

    }

    InputActionSettingsWindow::~InputActionSettingsWindow()
    {
        m_InputSystem->OnMouseAxisChange().Disconnect(this);
        m_InputSystem->OnMouseButton().Disconnect(this);
        m_InputSystem->OnMousePositionChange().Disconnect(this);
        m_InputSystem->OnKey().Disconnect(this);
        m_InputSystem->OnControllerAxisChange().Disconnect(this);
        m_InputSystem->OnControllerButton().Disconnect(this);
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
                        FilePath path;
                        if (FileSystem::FileDialog::OpenFileDialog(path, "Input actions asset", InputActions::InputActionsSerializer::Extensions))
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
                        FilePath path;
                        if (FileSystem::FileDialog::SaveFileDialog(path, "Input actions asset", InputActions::InputActionsSerializer::Extensions))
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
                HashMap<StringId32, InputActions::InputActionsMap>& actionMaps = m_EditableCopy->GetMaps();

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

        InputActions::InputBinding& selectedInputBinding = GetSelectedInputBinding();
        selectedInputBinding.SetInputBindingSlot(m_SelectedBindingSlotIndex, Input::InputID{ .ID = Enums::ToIntegral(event.Button) });
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

        InputActions::InputBinding& selectedInputBinding = GetSelectedInputBinding();
        selectedInputBinding.SetInputBindingSlot(m_SelectedBindingSlotIndex, Input::InputID{ .ID = Enums::ToIntegral(event.Key) });
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

        InputActions::InputBinding& selectedInputBinding = GetSelectedInputBinding();
        selectedInputBinding.SetInputBindingSlot(m_SelectedBindingSlotIndex, Input::InputID{ .ID = Enums::ToIntegral(event.Axis) });
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

        InputActions::InputBinding& selectedInputBinding = GetSelectedInputBinding();
        selectedInputBinding.SetInputBindingSlot(m_SelectedBindingSlotIndex, Input::InputID{ .ID = Enums::ToIntegral(event.Button) });
        m_IsListeningOnInput = false;
    }

    void InputActionSettingsWindow::RenderActionMaps(HashMap<StringId32, InputActions::InputActionsMap>& actionMaps)
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
            InputActions::InputActionsMap& selectedMap = m_EditableCopy->GetContext(m_SelectedActionMapId);

            if (ImGui::BeginPopupContextItem("###CreateInputAction", ImGuiPopupFlags_MouseButtonRight))
            {
                if (Ui::Button(Localization::Generic::Add))
                {
                    InputActions::InputAction& newAction = selectedMap.GetActions().emplace_back("New Action");
                    newAction.SetType(InputActions::ActionType::Bool);
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

                    DynamicArray<UniquePtr<InputActions::InputBinding>>& bindings = action.GetBindings();
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
        InputActions::InputActionsMap& selectedMap = m_EditableCopy->GetContext(m_SelectedActionMapId);
        DynamicArray<InputActions::InputAction>& selectedActions = selectedMap.GetActions();
        InputActions::InputAction& selectedAction = selectedActions[m_SelectedActionIndex];

        InputActions::ActionType actionType = selectedAction.GetType();

        Ui::PropertyGrid::DrawProperty("Action Id", selectedAction.GetId().GetString());

        if (Ui::PropertyGrid::DrawProperty<InputActions::ActionType, InputActions::ActionType::Invalid, InputActions::ActionType::Count>("Value Type", actionType))
        {
            selectedAction.GetBindings().clear();
            selectedAction.SetType(actionType);
        }
    }


    void InputActionSettingsWindow::OnInputAssetLoaded(Assets::AssetHandle<InputActions::InputActionsAsset> inputActionsAsset)
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


    InputActions::InputBinding& InputActionSettingsWindow::GetSelectedInputBinding()
    {
        InputActions::InputActionsMap& selectedMap = m_EditableCopy->GetContext(m_SelectedActionMapId);
        DynamicArray<InputActions::InputAction>& availableActions = selectedMap.GetActions();
        InputActions::InputAction& selectedAction = availableActions[m_SelectedActionIndex];
        DynamicArray<UniquePtr<InputActions::InputBinding>>& selectedActionBindings = selectedAction.GetBindings();
        InputActions::InputBinding& selectedInputBinding = *selectedActionBindings[m_SelectedBindingIndex];
        return selectedInputBinding;
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
