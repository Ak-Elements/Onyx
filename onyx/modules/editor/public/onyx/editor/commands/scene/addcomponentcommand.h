#pragma once

#include <onyx/editor/commands/scene/scenecommand.h>
#include <onyx/entity/entity.h>

namespace Onyx
{
    namespace Assets
    {
        struct AssetId;
    }

    namespace GameCore
    {
        class GameCoreSystem;
    }
}

namespace Onyx::Editor
{
    struct AddComponentCommand : public SceneCommand
    {
        AddComponentCommand(Entity::EntityId entity, StringId32 componentTypeId, Assets::AssetId sceneId, GameCore::GameCoreSystem& gameCoreSystem);

        void Execute() override;

    private:
        Entity::EntityId m_EntityId; 
        StringId32 m_ComponentTypeId;
    };
}