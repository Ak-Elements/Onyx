#pragma once

#include <editor/windows/editorwindow.h>

#include <onyx/input/inputactionsasset.h>
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

        InputActionSettingsWindow(Assets::AssetSystem& assetSystem, Localization::LocalizationModule& localizationModule, Input::InputSystem& inputSystem);
        ~InputActionSettingsWindow() override;

        StringView GetWindowId() override { return WindowId; }

        // TODO: Replace with an asset handle
        //void SetInputActionsFile(Reference<Application::InputActionsAsset>& actionMapAsset);
    private:
        void OnRender(Ui::ImGuiSystem& system) override;

        void OnInputEvent(const Input::InputEvent* inputEvent);

        void RenderActionMaps(HashMap<StringId32, Input::InputActionsMap>& actionMaps);
        void RenderInputActions();
        
        void RenderBindings(bool& isSelected, DynamicArray<UniquePtr<Input::InputBinding>>& bindings);
        void RenderBinding(bool& isSelected, onyxS32 bindingIndex, Input::InputBinding& binding);

        void RenderActionProperties();
        void RenderSelectedBindingProperties();

        void MarkAsDirty() { m_IsDirty = true; }

        void OnInputAssetLoaded(Reference<Input::InputActionsAsset>& inputActionsAsset);

    private:
        Assets::AssetSystem* m_AssetSystem = nullptr;
        Localization::LocalizationModule* m_LocalizationModule = nullptr;

        // Copy of InputActionAsset to edit until save
        Reference<Input::InputActionsAsset> m_EditableCopy;
      
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
