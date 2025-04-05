#pragma once

#include <onyx/engine/enginesystem.h>

namespace Onyx
{
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
        ~EditorSystem() override;

        void Init(GameCore::GameCoreSystem& gameCore,
            Ui::ImGuiSystem& imguiSystem,
            Assets::AssetSystem& assetSystem,
            Graphics::GraphicsApi& graphicsApi,
            Input::InputSystem& inputSystem,
            Input::InputActionSystem& inputActionSystem);
    };
}
