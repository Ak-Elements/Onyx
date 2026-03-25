#pragma once

#include <onyx/editor/windows/editorwindow.h>

#include <onyx/editor/commands/commandgraph.h>
#include <onyx/entity/entityregistry.h>
#include <onyx/input/inputevent.h>
#include <onyx/inputactions/inputactionsasset.h>
#include <onyx/localization/localizedstring.h>
#include <onyx/ui/imguiwindow.h>

namespace onyx::localization {
class LocalizationModule;
}

namespace onyx::assets {
class AssetSystem;
}

namespace onyx::input {
struct InputActionsMap;
class InputSystem;
class InputBinding;
struct InputActionContext;
} // namespace onyx::input

namespace onyx::input_actions {
struct InputActionEvent;
}

namespace onyx::editor {
class InputActionSettingsWindow : public ui::ImGuiWindow {
    friend struct InputActionCommand;

  public:
    static constexpr StringView WindowId = "InputActionSettings";
    static constexpr StringView WindowCategory = "Input";

    StringView GetWindowId() override { return WindowId; }

    StringId64 GetSelectedActionId() const { return m_SelectedActionId; }
    StringId64 GetSelectedBindingIndex() const { return m_SelectedBindingIndex; }

  private:
    void OnOpen() override;
    void OnRender( ui::ImGuiSystem& imguiSystem ) override;
    void OnClose() override;

    void OnMouseAxisChange( const input::MouseAxisEvent& event );
    void OnMouseButton( const input::MouseButtonEvent& event );
    void OnMousePositionChange( const input::MousePositionEvent& event );

    void OnKey( const input::KeyboardEvent& event );

    void OnControllerAxisChange( const input::GameControllerAxisEvent& event );
    void OnControllerButton( const input::GameControllerButtonEvent& event );

    void RenderInputActions();

    void RenderBindings( bool& isSelected, DynamicArray< UniquePtr< input_actions::InputBinding > >& bindings );
    void RenderBinding( bool& isSelected, int32_t bindingIndex, input_actions::InputBinding& binding );

    void RenderActionProperties();
    void RenderSelectedBindingProperties();

    void MarkAsDirty() { m_IsDirty = true; }

    void OnInputAssetLoaded( assets::AssetHandle< input_actions::InputActionsContext > inputActionsAsset );

    assets::AssetId GetOpenAssetId() const { return m_InputContextAssetId; }
    void BindInputBindingSlot( input::InputID inputId );

    input_actions::InputActionsMap& GetOpenActionsContext() { return m_OpenInputContext; }

    void OnDeleteAction( const input_actions::InputActionEvent& deleteAction );

  private:
    CommandGraph< input_actions::InputActionsMap > m_CommandsHistory;

    // Copy of InputActionAsset to edit until save
    assets::AssetId m_InputContextAssetId;
    input_actions::InputActionsMap m_OpenInputContext;

    DynamicArray< bool > m_MapsSelectedStates;
    DynamicArray< bool > m_ActionsSelectedStates;

    StringId64 m_SelectedActionId = StringId64::Invalid;
    int32_t m_SelectedActionIndex = InvalidIndex32;

    int32_t m_SelectedBindingIndex = InvalidIndex32;
    int32_t m_SelectedBindingSlotIndex = InvalidIndex32;

    bool m_IsListeningOnInput = false;
    bool m_IsDirty = false;
};
} // namespace onyx::editor
