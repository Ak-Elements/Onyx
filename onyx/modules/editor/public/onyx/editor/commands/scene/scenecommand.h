#pragma once

#include <onyx/editor/commands/command.h>
#include <onyx/assets/assetid.h>

namespace Onyx
{
    namespace Entity
    {
        class ComponentFactory;
    }

    namespace GameCore
    {
        class GameCoreSystem;
        class Scene;
    }
}

namespace Onyx::Editor
{
    struct SceneCommand : public ICommand
    {
        SceneCommand(StringId32 id, Assets::AssetId sceneId, GameCore::GameCoreSystem& gameCoreSystem);

    protected:
        GameCore::Scene& GetScene();
        const Entity::ComponentFactory& GetComponentFactory();
        
    private:
        Assets::AssetId m_SceneId = Assets::AssetId::Invalid;
        GameCore::GameCoreSystem* m_GameCoreSystem = nullptr;

    };
}