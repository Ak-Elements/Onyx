#pragma once

#include <onyx/editor/windows/editorwindow.h>

#include <onyx/inputactions/inputactionsasset.h>
#include <onyx/input/inputevent.h>
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

namespace Onyx::Editor
{
    class InputActionSettingsWindow : public Ui::ImGuiWindow
    {
    public:
        static constexpr StringView WindowId = "InputActionSettings";
        static constexpr StringView WindowCategory = "Input";
        
        StringView GetWindowId() override { return WindowId; }

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

        void RenderActionMaps(HashMap<StringId32, InputActions::InputActionsMap>& actionMaps);
        void RenderInputActions();
        
        void RenderBindings(bool& isSelected, DynamicArray<UniquePtr<InputActions::InputBinding>>& bindings);
        void RenderBinding(bool& isSelected, onyxS32 bindingIndex, InputActions::InputBinding& binding);

        void RenderActionProperties();
        void RenderSelectedBindingProperties();

        void MarkAsDirty() { m_IsDirty = true; }

        void OnInputAssetLoaded(Assets::AssetHandle<InputActions::InputActionsAsset> inputActionsAsset);

        InputActions::InputBinding& GetSelectedInputBinding();

    private:
        // Copy of InputActionAsset to edit until save
        Assets::AssetHandle<InputActions::InputActionsAsset> m_EditableCopy;
      
        DynamicArray<bool> m_MapsSelectedStates;
        DynamicArray<bool> m_ActionsSelectedStates;

        StringId32 m_SelectedActionMapId = 0;
        onyxS32 m_SelectedActionIndex = INVALID_INDEX_32;

        onyxS32 m_SelectedBindingIndex = INVALID_INDEX_32;
        onyxS32 m_SelectedBindingSlotIndex = INVALID_INDEX_32;

        bool m_IsListeningOnInput = false;
        bool m_IsDirty = false;
    };
}
