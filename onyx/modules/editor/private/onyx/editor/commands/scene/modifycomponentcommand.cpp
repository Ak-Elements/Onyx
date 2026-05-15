#include <onyx/editor/commands/scene/modifycomponentcommand.h>

#include <onyx/assets/assethandle.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>

#include <utility>

namespace onyx::editor {
ModifyComponentCommand::ModifyComponentCommand( ecs::EntityId entity,
                                                StringId32 componentTypeId,
                                                std::any&& component,
                                                assets::AssetId sceneId,
                                                game_core::GameCoreSystem& gameCoreSystem )
    : SceneCommand( "ModifyComponent", std::move( sceneId ), gameCoreSystem )
    , m_component( std::move( component ) )
    , m_entityId( entity )
    , m_componentTypeId( componentTypeId ) {}

void ModifyComponentCommand::execute() {
    const ecs::ComponentFactory& componentFactory = GetComponentFactory();
    ecs::EntityRegistry& registry = GetScene().getRegistry();

    const ecs::IComponentMeta* componentMeta = componentFactory.GetComponentMeta( m_componentTypeId )
                                                   .value_or( nullptr );
    ONYX_ASSERT( componentMeta != nullptr );

    if( entt::basic_sparse_set< ecs::EntityId >* componentStorage = registry.GetStorage(
            componentMeta->getRuntimeTypeId() ) ) {
        void* componentPtr = componentStorage->value( m_entityId );
        if( componentPtr == nullptr )
            return;

        bool hasCopied = componentFactory.TryCreateComponent( registry, m_entityId, m_componentTypeId, m_component );
        if( hasCopied == false ) {
            ONYX_LOG_WARNING( "Failed modifying component({}) on entity {}",
                              m_componentTypeId,
                              static_cast< uint32_t >( m_entityId ) );
        }
    }
}
} // namespace onyx::editor
