#pragma once

#include <onyx/editor/commands/scene/scenecommand.h>

namespace Onyx
{
    namespace Entity
    {
        enum class EntityId : onyxU32;
    }
}

namespace Onyx::Editor
{
    struct RenameEntityCommand : public SceneCommand
    {
    public:
        RenameEntityCommand(Entity::EntityId entity,
            String name,
            Assets::AssetId sceneId,
            GameCore::GameCoreSystem& gameCoreSystem);
            
        void Execute() override;

    private:
        String m_Name;
        Entity::EntityId m_Entity;
    };
}