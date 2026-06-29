#include <onyx/editor/commands/scene/addcomponentcommand.h>

#include <onyx/assets/assethandle.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>

namespace onyx::editor {
AddComponentCommand::AddComponentCommand( ecs::EntityId entity,
                                          StringId32 componentTypeId,
                                          assets::AssetId sceneId,
                                          game_core::GameCoreSystem& gameCoreSystem )
    : SceneCommand( "AddComponent", sceneId, gameCoreSystem )
    , m_entityId( entity )
    , m_componentTypeId( componentTypeId ) {}

void AddComponentCommand::execute() {
    const ecs::ComponentFactory& componentFactory = GetComponentFactory();
    ecs::EntityRegistry& registry = GetScene().getRegistry();
    componentFactory.tryCreateComponent( registry, m_entityId, m_componentTypeId );
}
} // namespace onyx::editor
