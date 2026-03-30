#pragma once

#include "onyx/defines.h"
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

    StringView getWindowId() override { return WindowId; }

    ONYX_NO_DISCARD StringId64 getSelectedActionId() const { return m_selectedActionId; }
    ONYX_NO_DISCARD int32_t getSelectedBindingIndex() const { return m_selectedBindingIndex; }

  private:
    void onOpen() override;
    void onRender( ui::ImGuiSystem& imguiSystem ) override;
    void onClose() override;

    void onRenderMainMenuBar() override;

    void onMouseAxisChange( const input::MouseAxisEvent& event );
    void onMouseButton( const input::MouseButtonEvent& event );
    void onMousePositionChange( const input::MousePositionEvent& event );

    void onKey( const input::KeyboardEvent& event );

    void onControllerAxisChange( const input::GameControllerAxisEvent& event );
    void onControllerButton( const input::GameControllerButtonEvent& event );

    void renderInputActions();

    void renderBindings( bool& isSelected, DynamicArray< UniquePtr< input_actions::InputBinding > >& bindings );
    void renderBinding( bool& isSelected, int32_t bindingIndex, input_actions::InputBinding& binding );

    void renderActionProperties();
    void renderSelectedBindingProperties();

    void markAsDirty() { m_isDirty = true; }

    void onInputAssetLoaded( assets::AssetHandle< input_actions::InputActionsContext > inputActionsAsset );

    ONYX_NO_DISCARD assets::AssetId getOpenAssetId() const { return m_inputContextAssetId; }
    void bindInputBindingSlot( input::InputID inputId );

    input_actions::InputActionsMap& getOpenActionsContext() { return m_openInputContext; }

    void onDeleteAction( const input_actions::InputActionEvent& deleteAction );

  private:
    CommandGraph< input_actions::InputActionsMap > m_commandsHistory;

    // Copy of InputActionAsset to edit until save
    assets::AssetId m_inputContextAssetId;
    input_actions::InputActionsMap m_openInputContext;

    DynamicArray< bool > m_mapsSelectedStates;
    DynamicArray< bool > m_actionsSelectedStates;

    StringId64 m_selectedActionId = StringId64::Invalid;
    int32_t m_selectedActionIndex = InvalidIndex32;

    int32_t m_selectedBindingIndex = InvalidIndex32;
    int32_t m_selectedBindingSlotIndex = InvalidIndex32;

    bool m_isListeningOnInput = false;
    bool m_isDirty = false;
};
} // namespace onyx::editor
