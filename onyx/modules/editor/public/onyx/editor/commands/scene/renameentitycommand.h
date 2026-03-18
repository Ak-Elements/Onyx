#pragma once

#include <onyx/editor/commands/scene/scenecommand.h>

namespace onyx
{
    namespace ecs
    {
        enum class EntityId : onyxU32;
    }
}

namespace onyx::editor
{
    struct RenameEntityCommand : public SceneCommand
    {
    public:
        RenameEntityCommand(ecs::EntityId entity,
            String name,
            assets::AssetId sceneId,
            game_core::GameCoreSystem& gameCoreSystem);
            
        void Execute() override;

    private:
        String m_Name;
        ecs::EntityId m_Entity;
    };
}