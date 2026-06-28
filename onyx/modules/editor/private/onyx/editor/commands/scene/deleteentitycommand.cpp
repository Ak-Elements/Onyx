#include <onyx/editor/commands/scene/deleteentitycommand.h>

#include <onyx/assets/assethandle.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>

namespace onyx::editor {
DeleteEntityCommand::DeleteEntityCommand( ecs::EntityId entityId,
                                          assets::AssetId sceneId,
                                          game_core::GameCoreSystem& gameCoreSystem )
    : SceneCommand( "DeleteEntity", sceneId, gameCoreSystem )
    , m_EntityId( entityId ) {}

void DeleteEntityCommand::execute() {
    ecs::EntityRegistry& registry = GetScene().getRegistry();
    registry.deleteEntity( m_EntityId );
}
} // namespace onyx::editor
