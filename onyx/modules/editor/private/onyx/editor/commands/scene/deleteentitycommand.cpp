#include <onyx/editor/commands/scene/deleteentitycommand.h>

#include <onyx/assets/assethandle.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>

namespace Onyx::Editor
{
    DeleteEntityCommand::DeleteEntityCommand(Entity::EntityId entityId, Assets::AssetId sceneId, GameCore::GameCoreSystem& gameCoreSystem)
        : SceneCommand("DeleteEntity", sceneId, gameCoreSystem)
        , m_EntityId(entityId)
    {
    }

    void DeleteEntityCommand::Execute()
    {
        Entity::EntityRegistry& registry = GetScene().GetRegistry();
        registry.DeleteEntity(m_EntityId);
    }
}