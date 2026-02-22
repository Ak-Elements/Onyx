#pragma once

#include <onyx/editor/windows/editorwindow.h>

#include <onyx/inputactions/inputactionsasset.h>
#include <onyx/input/inputevent.h>
#include <onyx/editor/commands/commandgraph.h>
#include <onyx/entity/entityregistry.h>
#include <onyx/localization/localizedstring.h>
#include <onyx/ui/imguiwindow.h>

namespace Onyx::Localization
{
    class LocalizationModule;
}

namespace Onyx::Assets
{
    class AssetSystem;
}

namespace Onyx::Input
{
    struct InputActionsMap;
    class InputSystem;
    class InputBinding;
    struct InputActionContext;
}

namespace Onyx::InputActions
{
    struct InputActionEvent;
}

namespace Onyx::Editor
{
    class InputActionSettingsWindow : public Ui::ImGuiWindow
    {
        friend struct InputActionCommand;
    public:
        static constexpr StringView WindowId = "InputActionSettings";
        static constexpr StringView WindowCategory = "Input";
        
        StringView GetWindowId() override { return WindowId; }

        StringId64 GetSelectedActionId() const { return m_SelectedActionId; }
        StringId64 GetSelectedBindingIndex() const { return m_SelectedBindingIndex; }
    private:
        void OnOpen() override;
        void OnRender(Ui::ImGuiSystem& imguiSystem) override;
        void OnClose() override;

        void OnMouseAxisChange(const Input::MouseAxisEvent& event);
        void OnMouseButton(const Input::MouseButtonEvent& event);
        void OnMousePositionChange(const Input::MousePositionEvent& event);

        void OnKey(const Input::KeyboardEvent& event);

        void OnControllerAxisChange(const Input::GameControllerAxisEvent& event);
        void OnControllerButton(const Input::GameControllerButtonEvent& event);

        void RenderInputActions();
        
        void RenderBindings(bool& isSelected, DynamicArray<UniquePtr<InputActions::InputBinding>>& bindings);
        void RenderBinding(bool& isSelected, onyxS32 bindingIndex, InputActions::InputBinding& binding);

        void RenderActionProperties();
        void RenderSelectedBindingProperties();

        void MarkAsDirty() { m_IsDirty = true; }

        void OnInputAssetLoaded(Assets::AssetHandle<InputActions::InputActionsContext> inputActionsAsset);

        Assets::AssetId GetOpenAssetId() const { return m_InputContextAssetId; }
        void BindInputBindingSlot(Input::InputID inputId);

        InputActions::InputActionsMap& GetOpenActionsContext() { return m_OpenInputContext; }

        void OnDeleteAction(const InputActions::InputActionEvent& deleteAction);

    private:
        CommandGraph<InputActions::InputActionsMap> m_CommandsHistory;

        // Copy of InputActionAsset to edit until save
        Assets::AssetId m_InputContextAssetId;
        InputActions::InputActionsMap m_OpenInputContext;
      
        DynamicArray<bool> m_MapsSelectedStates;
        DynamicArray<bool> m_ActionsSelectedStates;

        StringId64 m_SelectedActionId = StringId64::Invalid;
        onyxS32 m_SelectedActionIndex = INVALID_INDEX_32;

        onyxS32 m_SelectedBindingIndex = INVALID_INDEX_32;
        onyxS32 m_SelectedBindingSlotIndex = INVALID_INDEX_32;

        bool m_IsListeningOnInput = false;
        bool m_IsDirty = false;
    };
}
