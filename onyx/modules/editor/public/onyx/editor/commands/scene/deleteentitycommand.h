#pragma once

#include <onyx/editor/commands/scene/scenecommand.h>
#include <onyx/entity/entity.h>

namespace Onyx::Editor
{
    struct DeleteEntityCommand : public SceneCommand
    {
    public:
        DeleteEntityCommand(Entity::EntityId entityId, Assets::AssetId sceneId, GameCore::GameCoreSystem& gameCoreSystem);
            
        void Execute() override;

    private:
        Entity::EntityId m_EntityId = Entity::EntityId::Invalid;
    };
}