#include <onyx/editor/commands/scene/deleteentitycommand.h>

#include <onyx/assets/assethandle.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>

namespace onyx::editor
{
    DeleteEntityCommand::DeleteEntityCommand(ecs::EntityId entityId, assets::AssetId sceneId, game_core::GameCoreSystem& gameCoreSystem)
        : SceneCommand("DeleteEntity", sceneId, gameCoreSystem)
        , m_EntityId(entityId)
    {
    }

    void DeleteEntityCommand::Execute()
    {
        ecs::EntityRegistry& registry = GetScene().GetRegistry();
        registry.DeleteEntity(m_EntityId);
    }
}