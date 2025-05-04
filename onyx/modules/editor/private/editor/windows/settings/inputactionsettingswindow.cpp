#include <editor/windows/settings/inputactionsettingswindow.h>

#include <onyx/input/inputactionsmap.h>
#include <onyx/input/mouse.h>
#include <onyx/input/keycodes.h>
#include <onyx/input/gamepad.h>
#include <onyx/input/inputbindingsregistry.h>
#include <onyx/input/inputsystem.h>
#include <onyx/input/inputactionsserializer.h>
#include <onyx/assets/assetsystem.h>
#include <onyx/input/inputaction.h>

#include <onyx/filesystem/filedialog.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <onyx/ui/imguistyle.h>
#include <onyx/ui/widgets.h>

#include <imgui_internal.h>
#include <imgui_stacklayout.h>

namespace Onyx::Editor
{
    InputActionSettingsWindow::InputActionSettingsWindow(Assets::AssetSystem& assetSystem, Input::InputSystem& inputSystem)
        : m_AssetSystem(&assetSystem)
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

        const char* windowName = Format::Format("{}{} - [Input Actions]###InputActionsEditor", m_IsDirty ? "*" : "", assetName.data());
        SetName(windowName);

        if (Begin())
        {
            if (BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Open"))
                    {
                        FileSystem::Filepath path;
                        if (FileSystem::FileDialog::OpenFileDialog(path, "Input actions asset", Input::InputActionsSerializer::Extensions))
                        {
                            Assets::AssetId assetId(path);
                            m_AssetSystem->GetAssetUnmanaged(assetId, m_EditableCopy);
                            m_EditableCopy->GetOnLoadedEvent().Connect<&InputActionSettingsWindow::OnInputAssetLoaded>(this);
                        }
                    }

                    if (ImGui::MenuItem("Save"))
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
                    ImGui::BeginVertical("##InputActionMap::InputAction::Data", ImGui::GetContentRegionAvail());

                    if (m_SelectedActionMapId.IsValid() && (m_SelectedActionIndex != INVALID_INDEX_32))
                        RenderActionProperties();

                    if (m_SelectedBindingIndex != INVALID_INDEX_32)
                        RenderSelectedBindingProperties();

                    ImGui::EndVertical();

                    ImGui::EndTable();
                }
            }
        }

        ImGui::End();
    }

    void InputActionSettingsWindow::OnInputEvent(const Input::InputEvent* inputEvent)
    {
        if (m_IsListeningOnInput)
        {
            if (m_SelectedBindingIndex == INVALID_INDEX_32)
            {
                m_IsListeningOnInput = false;
                return;
            }

            if ((inputEvent->m_Id == Input::InputEventId::MousePositionChanged) || (inputEvent->m_Id == Input::InputEventId::MouseWheel))
                return;

            Input::InputActionsMap& selectedMap = m_EditableCopy->GetContext(m_SelectedActionMapId);
            DynamicArray<Input::InputAction>& availableActions = selectedMap.GetActions();
            Input::InputAction& selectedAction = availableActions[m_SelectedActionIndex];
            DynamicArray<UniquePtr<Input::InputBinding>>& selectedActionBindings = selectedAction.GetBindings();
            UniquePtr<Input::InputBinding>& selectedInputBinding = selectedActionBindings[m_SelectedBindingIndex];

            const bool isAxisNonComposite = ((selectedInputBinding->GetTypeId() == Input::InputBindingAxis1D::TypeId) ||
                (selectedInputBinding->GetTypeId() == Input::InputBindingAxis2D::TypeId) ||
                (selectedInputBinding->GetTypeId() == Input::InputBindingAxis3D::TypeId));

            if (isAxisNonComposite == false)
            {
                if (inputEvent->IsMouseEvent())
                {
                    const Input::MouseEvent* mouseEvent = static_cast<const Input::MouseEvent*>(inputEvent);

                    if (selectedInputBinding->GetInputType() != Input::InputType::Mouse)
                    {
                        selectedInputBinding->SetInputType(Input::InputType::Mouse);
                        selectedInputBinding->Reset();
                    }

                    
                    selectedInputBinding->SetInputBindingSlot(m_SelectedBindingSlotIndex, Enums::ToIntegral(mouseEvent->m_Button));
                }
                else if (inputEvent->IsKeyboardEvent())
                {
                    const Input::KeyboardEvent* keyboardEvent = static_cast<const Input::KeyboardEvent*>(inputEvent);

                    if (selectedInputBinding->GetInputType() != Input::InputType::Keyboard)
                    {
                        selectedInputBinding->SetInputType(Input::InputType::Keyboard);
                        selectedInputBinding->Reset();
                    }

                    selectedInputBinding->SetInputBindingSlot(m_SelectedBindingSlotIndex, Enums::ToIntegral(keyboardEvent->m_Key));
                }
                else if (inputEvent->IsGamepadButtonEvent())
                {
                    const Input::GameControllerButtonEvent* gamepadButtonEvent = static_cast<const Input::GameControllerButtonEvent*>(inputEvent);


                    if (selectedInputBinding->GetInputType() != Input::InputType::Gamepad)
                    {
                        selectedInputBinding->SetInputType(Input::InputType::Gamepad);
                        selectedInputBinding->Reset();
                    }

                    selectedInputBinding->SetInputBindingSlot(m_SelectedBindingSlotIndex, Enums::ToIntegral(gamepadButtonEvent->m_Button));
                }
                
                m_IsListeningOnInput = false;
            }
        }
    }

    void InputActionSettingsWindow::RenderActionMaps(HashMap<StringId32, Input::InputActionsMap>& actionMaps)
    {
        // all action maps of the project
        ImGui::BeginListBox("##InputActionMaps", ImGui::GetContentRegionAvail());

        onyxU32 i = 0;
        for (auto&& [key, actionMap] : actionMaps)
        {
            if (ImGui::Selectable(actionMap.GetName().c_str(), m_MapsSelectedStates[i++], ImGuiSelectableFlags_SelectOnClick | ImGuiSelectableFlags_SpanAvailWidth))
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
        Ui::ScopedImGuiId id("##InputActionMap::InputActions");
        if (m_SelectedActionMapId.IsValid())
        {
            onyxS32 i = 0;
            Input::InputActionsMap& selectedMap = m_EditableCopy->GetContext(m_SelectedActionMapId);

            if (ImGui::BeginPopupContextItem("##CreateInputAction", ImGuiPopupFlags_MouseButtonRight))
            {
                if (ImGui::Button("Add Action"))
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
                        const HashSet<StringId32>& registeredBindings = Input::InputBindingsRegistry::GetRegisteredBindings(action.GetType());
                        for (StringId32 bindingTypeId : registeredBindings)
                        {

                            StringView buttonLabel = Format::Format("Add {}", Input::InputBindingsRegistry::GetBindingName(bindingTypeId));
                            if (ImGui::MenuItem(buttonLabel.data()))
                            {
                                action.GetBindings().emplace_back(Input::InputBindingsRegistry::CreateBinding(bindingTypeId));
                                ImGui::CloseCurrentPopup();
                            }
                        }

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
            UniquePtr<Input::InputBinding>& binding = bindings[bindingIndex];

            bool isBindingSelected = isSelected && (m_SelectedBindingIndex == bindingIndex);
            //bool wasSelected = isSelected;

            StringView bindingInputTypeLabel = Enums::ToString(binding->GetInputType());
            onyxS32 bindingSlotsCount = binding->GetInputBindingSlotsCount();
            const bool hasCollapsibleHeader = bindingSlotsCount > 1;
            bool showBindings = true;
            if (hasCollapsibleHeader)
            {
                ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_SpanAvailWidth;
                if (isSelected)
                    treeNodeFlags |= ImGuiTreeNodeFlags_Selected;

                StringView label = Format::Format("{} [{}]", binding->GetName(), bindingInputTypeLabel);
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
                    showBindings = false;
                }
            }

            if (showBindings)
            {
                Ui::ScopedImGuiIndent indent(hasCollapsibleHeader ? 20.0f : 4.0f);
                for (onyxS32 slotIndex = 0; slotIndex < bindingSlotsCount; ++slotIndex)
                {
                    bool isBindingSlotSelected = isBindingSelected && (m_SelectedBindingSlotIndex == slotIndex);

                    StringView bindingSlotLabel = binding->GetInputBindingSlotName(slotIndex);
                    StringView boundInputLabel;
                    onyxU32 boundInputValue = binding->GetBoundInputForSlot(slotIndex);

                    if (boundInputValue == 0)
                    {
                        boundInputLabel = "Unbound";
                    }
                    else
                    {
                        switch (binding->GetInputType())
                        {
                            case Input::InputType::Mouse:
                            {
                                boundInputLabel = Enums::ToString(static_cast<Input::MouseButton>(boundInputValue));
                                break;
                            }
                            case Input::InputType::Keyboard:
                            {
                                boundInputLabel = Enums::ToString(static_cast<Input::Key>(boundInputValue));
                                break;
                            }
                            case Input::InputType::Gamepad:
                            {
                                boundInputLabel = Enums::ToString(static_cast<Input::GameControllerButton>(boundInputValue));
                                break;
                            }
                        }
                    }

                    StringView inputBindingSlotLabel;
                    if (hasCollapsibleHeader)
                    {
                        inputBindingSlotLabel = Format::Format("{}{} {}", bindingSlotLabel, bindingSlotLabel.empty() ? "" : ":", boundInputLabel);
                    }
                    else
                    {
                        inputBindingSlotLabel = Format::Format("{}{} {} [{}]", bindingSlotLabel, bindingSlotLabel.empty() ? "" : ":", boundInputLabel, bindingInputTypeLabel);
                    }

                    if (ImGui::Selectable(inputBindingSlotLabel.data(), isBindingSlotSelected))
                    {
                        isSelected = true;
                        isBindingSelected = true;
                        m_SelectedBindingIndex = bindingIndex;
                        m_SelectedBindingSlotIndex = slotIndex;
                    }
                }
            }
        }
    }

    void InputActionSettingsWindow::RenderActionProperties()
    {
        Input::InputActionsMap& selectedMap = m_EditableCopy->GetContext(m_SelectedActionMapId);
        DynamicArray<Input::InputAction>& selectedActions = selectedMap.GetActions();
        Input::InputAction& selectedAction = selectedActions[m_SelectedActionIndex];

        Input::ActionType actionType = selectedAction.GetType();

        ImGui::TextEx((selectedAction.GetId().GetString().data()));

        if (ImGui::BeginCombo("Action type", Enums::ToString(actionType).data()))
        {
            for (Input::ActionType type = Input::ActionType::Invalid; type < Input::ActionType::Count; ++type)
            {
                if (type == Input::ActionType::Invalid)
                    continue;

                if (ImGui::Selectable(Enums::ToString(type).data(), false))
                {
                    selectedAction.GetBindings().clear();
                }
            }

            ImGui::EndCombo();
        }
    }

    void InputActionSettingsWindow::RenderSelectedBindingProperties()
    {
        if (m_SelectedBindingIndex == INVALID_INDEX_32)
            return;

        if (ImGui::Button("Listen"))
        {
            m_IsListeningOnInput = true;
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
    }
}
