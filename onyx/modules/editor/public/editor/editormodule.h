#pragma once

#include <onyx/engine/enginesystem.h>

namespace Onyx
{
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
        class GraphicsApi;
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
        StringId32 GetTypeId() const { return TypeId; }

        EditorSystem();
        ~EditorSystem() override;

        void Init(GameCore::GameCoreSystem& gameCore,
            Ui::ImGuiSystem& imguiSystem,
            Assets::AssetSystem& assetSystem,
            Graphics::GraphicsApi& graphicsApi,
            Input::InputSystem& inputSystem,
            Input::InputActionSystem& inputActionSystem,
            Localization::LocalizationModule& locaModule);
    };
}
