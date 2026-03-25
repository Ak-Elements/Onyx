#include <onyx/editor/commands/scene/modifycomponentcommand.h>

#include <onyx/assets/assethandle.h>

#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>

namespace onyx::editor {
ModifyComponentCommand::ModifyComponentCommand( ecs::EntityId entity,
                                                StringId32 componentTypeId,
                                                DynamicArray< uint32_t >&& componentData,
                                                assets::AssetId sceneId,
                                                game_core::GameCoreSystem& gameCoreSystem )
    : SceneCommand( "ModifyComponent", sceneId, gameCoreSystem )
    , m_ComponentData( std::move( componentData ) )
    , m_EntityId( entity )
    , m_ComponentTypeId( componentTypeId ) {}

void ModifyComponentCommand::Execute() {
    const ecs::ComponentFactory& componentFactory = GetComponentFactory();
    ecs::EntityRegistry& registry = GetScene().GetRegistry();

    const ecs::IComponentMeta* componentMeta = componentFactory.GetComponentMeta( m_ComponentTypeId )
                                                   .value_or( nullptr );
    ONYX_ASSERT( componentMeta != nullptr );

    if ( entt::basic_sparse_set< ecs::EntityId >* componentStorage = registry.GetStorage(
             componentMeta->GetRuntimeTypeId() ) ) {
        void* componentPtr = componentStorage->value( m_EntityId );
        if ( componentPtr == nullptr )
            return;

        bool hasCopied = componentFactory.TryCreateComponent( registry,
                                                              m_EntityId,
                                                              m_ComponentTypeId,
                                                              m_ComponentData );
        if ( hasCopied == false ) {
            ONYX_LOG_WARNING( "Failed modifying component({}) on entity {}",
                              m_ComponentTypeId,
                              static_cast< uint32_t >( m_EntityId ) );
        }
    }
}
} // namespace onyx::editor