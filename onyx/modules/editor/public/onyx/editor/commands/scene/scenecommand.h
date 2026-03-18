#pragma once

#include <onyx/editor/commands/command.h>
#include <onyx/assets/assetid.h>

namespace onyx
{
    namespace ecs
    {
        class ComponentFactory;
    }

    namespace game_core
    {
        class GameCoreSystem;
        class Scene;
    }
}

namespace onyx::editor
{
    struct SceneCommand : public ICommand
    {
        SceneCommand(StringId32 id, assets::AssetId sceneId, game_core::GameCoreSystem& gameCoreSystem);

    protected:
        game_core::Scene& GetScene();
        const ecs::ComponentFactory& GetComponentFactory();
        
    private:
        assets::AssetId m_SceneId = assets::AssetId::Invalid;
        game_core::GameCoreSystem* m_GameCoreSystem = nullptr;

    };
}