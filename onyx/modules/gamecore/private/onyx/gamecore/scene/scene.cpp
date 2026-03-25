#include <onyx/gamecore/scene/scene.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/gamecore/components/namecomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/scene/scenesector.h>
#include <onyx/graphics/rendergraph/rendergraph.h>

#include <onyx/engine/enginesystem.h>
#include <onyx/entity/componentfactory.h>
#include <onyx/gamecore/gamecore.h>

namespace onyx::game_core {
Reference< Scene > Scene::create( IEngine& engine ) {
    GameCoreSystem& gameCoreSystem = engine.getSystem< GameCoreSystem >();
    return Reference< Scene >::create( gameCoreSystem.GetComponentFactory() );
}

Scene::Scene( ecs::ComponentFactory& factory )
    : m_Registry( factory ) {
    m_Registry.GetRegistry().on_construct< TransformComponent >().connect< &Scene::OnTransformComponentConstructed >(
        this );
    m_Registry.GetRegistry().on_destroy< TransformComponent >().connect< &Scene::OnTransformComponentDestroyed >(
        this );
}

void Scene::SetLoadCenter( const Vector3f32& loadCenter ) {
    if ( loadCenter != m_LoadCenter ) {
        m_LoadCenter = loadCenter;
    }
}

void Scene::SetStreamInDistance( float64 distance ) {
    m_SectorStreamer.SetStreamInDistance( distance );
}

void Scene::SetStreamOutDistance( float64 distance ) {
    m_SectorStreamer.SetStreamOutDistance( distance );
}

void Scene::Copy( const Reference< Scene >& fromScene ) {
    m_Registry.Clear();

    // all entities
    const ecs::EntityRegistry& fromRegistry = fromScene->GetRegistry();
    for ( ecs::EntityId sourceEntityId : fromRegistry.GetView< ecs::EntityId >() ) {
        ecs::EntityId targetEntity = m_Registry.CreateEntity();
        // create a copy of an entity component by component
        for ( auto&& componentStorageIt : fromRegistry.GetStorage() ) {
            if ( auto& componentStorage = componentStorageIt.second; componentStorage.contains( sourceEntityId ) ) {
                entt::meta_type metaClass = entt::resolve( componentStorageIt.first );

                if ( !metaClass ) {
                    ONYX_LOG_ERROR( "Failed copying component of entity during scene copy." );
                    continue;
                }

                const entt::meta_any componentHandle = metaClass.from_void( componentStorage.value( sourceEntityId ) );

                std::ignore = metaClass.construct( entt::forward_as_meta( m_Registry.GetRegistry() ),
                                                   targetEntity,
                                                   componentHandle );
            }
        }
    }
}

void Scene::Update( uint64_t /*deltaTime*/ ) {
    m_SectorStreamer.Update( m_LoadCenter );
}

String Scene::GetUniqueEntityName( const String& preferredName ) {
    auto namesView = m_Registry.GetView< const NameComponent >();

    uint32_t count = 0;
    for ( ecs::EntityId entityId : namesView ) {
        const NameComponent& nameComponent = namesView.get< const NameComponent >( entityId );

        if ( nameComponent.Name.starts_with( preferredName ) ) {
            ++count;
        }
    }

    if ( count > 0 ) {
        return format::format( "{}_{}", preferredName, count );
    }

    return preferredName;
}

void Scene::OnTransformComponentConstructed( ecs::EntityRegistry::EntityRegistryT& /*registry*/,
                                             ecs::EntityId entity ) {
    // if (m_Registry.HasComponents<TransientComponent>(entity))
    //{
    //     return;
    // }

    m_SectorStreamer.AddEntity( entity );
}

void Scene::OnTransformComponentDestroyed( ecs::EntityRegistry::EntityRegistryT& /*registry*/, ecs::EntityId entity ) {
    // if (m_Registry.HasComponents<TransientComponent>(entity))
    //{
    //     return;
    // }

    m_SectorStreamer.RemoveEntity( entity );
}
} // namespace onyx::game_core
