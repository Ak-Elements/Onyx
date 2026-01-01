#pragma once
#include <onyx/engine/enginesystem.h>

namespace Onyx
{
    namespace Volume
    {
        struct TerrainSettingsComponent;
    }

    namespace Localization
    {
        class LocalizationModule;
    }

    namespace Assets
    {
        class AssetSystem;
    }

    namespace GameCore
    {
        class GameCoreSystem;
    }

    namespace Input
    {
        class InputSystem;
        class InputActionSystem;
    }

    namespace Graphics
    {
        class GraphicsSystem;
    }

    namespace Ui
    {
        class ImGuiSystem;
    }
}

namespace Onyx::Editor
{
    class EditorSystem : public IEngineSystem
    {
    public:
        static constexpr StringId32 TypeId = "Onyx::Editor::EditorModule";
        StringId32 GetTypeId() const override { return TypeId; }

        EditorSystem(GameCore::GameCoreSystem& gameCore,
            Ui::ImGuiSystem& imguiSystem,
            Assets::AssetSystem& assetSystem,
            Graphics::GraphicsSystem& graphicsSystem,
            Input::InputSystem& inputSystem,
            Input::InputActionSystem& inputActionSystem,
            Localization::LocalizationModule& localizationModule);

        ~EditorSystem() override;
    };
}
