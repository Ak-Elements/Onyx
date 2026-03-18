#pragma once

#include <onyx/editor/commands/scene/scenecommand.h>
#include <onyx/entity/entity.h>

namespace onyx::editor
{
    struct DeleteEntityCommand : public SceneCommand
    {
    public:
        DeleteEntityCommand(ecs::EntityId entityId, assets::AssetId sceneId, game_core::GameCoreSystem& gameCoreSystem);
            
        void Execute() override;

    private:
        ecs::EntityId m_EntityId = ecs::EntityId::Invalid;
    };
}