#include <onyx/gamecore/scene/scene.h>

#include <onyx/filesystem/onyxfile.h>
#include <onyx/gamecore/components/namecomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/scene/scenesector.h>
#include <onyx/graphics/rendergraph/rendergraph.h>

#include <onyx/engine/enginesystem.h>
#include <onyx/entity/componentfactory.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/physics/physicssystem.h>

namespace onyx::game_core {
Reference< Scene > Scene::create( IEngine& engine ) {
    GameCoreSystem& gameCoreSystem = engine.getSystem< GameCoreSystem >();
    physics::PhysicsSystem& physicsSystem = engine.getSystem< physics::PhysicsSystem >();
    return Reference< Scene >::create( gameCoreSystem.getComponentFactory(), physicsSystem );
}

Scene::Scene( ecs::ComponentFactory& factory, onyx::physics::PhysicsSystem& physicsSystem )
    : m_registry( factory )
    , m_physicsWorld( physicsSystem.createPhysicsWorld3d() ) {
    m_registry.getRegistry().on_construct< TransformComponent >().connect< &Scene::onTransformComponentConstructed >(
        this );
    m_registry.getRegistry().on_destroy< TransformComponent >().connect< &Scene::onTransformComponentDestroyed >(
        this );
}

void Scene::setLoadCenter( const Vector3f32& loadCenter ) {
    if( loadCenter != m_loadCenter ) {
        m_loadCenter = loadCenter;
    }
}

void Scene::setStreamInDistance( float64 distance ) {
    m_sectorStreamer.SetStreamInDistance( distance );
}

void Scene::setStreamOutDistance( float64 distance ) {
    m_sectorStreamer.SetStreamOutDistance( distance );
}

void Scene::copy( const Reference< Scene >& fromScene ) {
    m_registry.clear();

    // all entities
    const ecs::EntityRegistry& fromRegistry = fromScene->getRegistry();
    for( ecs::EntityId sourceEntityId : fromRegistry.getView< ecs::EntityId >() ) {
        ecs::EntityId targetEntity = m_registry.createEntity();
        // create a copy of an entity component by component
        for( auto&& componentStorageIt : fromRegistry.getStorage() ) {
            if( auto& componentStorage = componentStorageIt.second; componentStorage.contains( sourceEntityId ) ) {
                entt::meta_type metaClass = entt::resolve( componentStorageIt.first );

                if( !metaClass ) {
                    ONYX_LOG_ERROR( "Failed copying component of entity during scene copy." );
                    continue;
                }

                const entt::meta_any componentHandle = metaClass.from_void( componentStorage.value( sourceEntityId ) );

                std::ignore = metaClass.construct( entt::forward_as_meta( m_registry.getRegistry() ),
                                                   targetEntity,
                                                   componentHandle );
            }
        }
    }
}

void Scene::update( uint64_t /*deltaTime*/ ) {
    m_sectorStreamer.Update( m_loadCenter );
}

String Scene::getUniqueEntityName( const String& preferredName ) {
    auto namesView = m_registry.getView< const NameComponent >();

    uint32_t count = 0;
    for( ecs::EntityId entityId : namesView ) {
        const NameComponent& nameComponent = namesView.get< const NameComponent >( entityId );

        if( nameComponent.Name.starts_with( preferredName ) ) {
            ++count;
        }
    }

    if( count > 0 ) {
        return format::format( "{}_{}", preferredName, count );
    }

    return preferredName;
}

void Scene::onTransformComponentConstructed( ecs::EntityRegistry::EntityRegistryT& /*registry*/,
                                             ecs::EntityId entity ) {
    // if (m_Registry.HasComponents<TransientComponent>(entity))
    //{
    //     return;
    // }

    m_sectorStreamer.AddEntity( entity );
}

void Scene::onTransformComponentDestroyed( ecs::EntityRegistry::EntityRegistryT& /*registry*/, ecs::EntityId entity ) {
    // if (m_Registry.HasComponents<TransientComponent>(entity))
    //{
    //     return;
    // }

    m_sectorStreamer.RemoveEntity( entity );
}
} // namespace onyx::game_core
