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
    , m_EntityId( entity )
    , m_ComponentTypeId( componentTypeId ) {}

void AddComponentCommand::execute() {
    const ecs::ComponentFactory& componentFactory = GetComponentFactory();
    ecs::EntityRegistry& registry = GetScene().getRegistry();
    componentFactory.TryCreateComponent( registry, m_EntityId, m_ComponentTypeId );
}
} // namespace onyx::editor
