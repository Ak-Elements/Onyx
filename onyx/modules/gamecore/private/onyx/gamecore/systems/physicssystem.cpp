#include <onyx/gamecore/systems/physicssystem.h>

#include <onyx/colors/base.h>
#include <onyx/entity/entitycommandbuffer.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/components/collision/boxshapecomponent.gen.h>
#include <onyx/gamecore/components/collision/capsuleshapecomponent.gen.h>
#include <onyx/gamecore/components/collision/sphereshapecomponent.gen.h>
#include <onyx/gamecore/components/physics/rigidbodycomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/graphics/debug/debugdrawqueue.h>
#include <onyx/physics/bodyid.h>
#include <onyx/physics/character.h>
#include <onyx/physics/motiontype.h>
#include <onyx/physics/physicsworld3d.h>

namespace onyx::game_core::physics {
namespace {
template < typename T >
void removeIfPresent( ecs::EntityRegistry& registry, ecs::EntityId entity ) {
    if( registry.hasComponent< T >( entity ) ) {
        registry.removeComponent< T >( entity );
    }
}

template < typename Add, typename... Remove >
void addMotionTypeTag( ecs::EntityRegistry& registry, ecs::EntityId entity ) {
    registry.addComponent< Add >( entity );
    ( removeIfPresent< Remove >( registry, entity ), ... );
}
} // namespace

struct InitPhysics {};

namespace stream_in {

// clang-format off
using RigidBodyAccess = ecs::Access
    ::Read< TransformComponent >
    ::Read< RigidBodyComponent >
    ::ReadIfExists< BoxShapeComponent, CapsuleShapeComponent, SphereShapeComponent >
    ::ReadIfExists< components::BodyId >
    ::With< InitPhysics >;
// clang-format on

using RigidBodyEntity = RigidBodyAccess::AsEntity;

void system( RigidBodyEntity entity,
             onyx::physics::PhysicsWorld3d& physicsSystem,
             ecs::EntityCommandBuffer entityCommandBuffer ) {
    auto&& [ transform, rigidbody ] = entity;

    auto physicsBodyID = entity.tryGetComponent< components::BodyId >();
    if( physicsBodyID ) {
        physicsSystem.removeCollider( physicsBodyID->Id );
    }

    // TODO: how do we prevent the presence of multiple shape components
    onyx::physics::BodyId bodyId{ onyx::physics::BodyId::Invalid };
    if( const auto* sphere = entity.tryGetComponent< SphereShapeComponent >() ) {
        bodyId = physicsSystem.createSphereCollider( transform.Translation,
                                                     transform.Rotation,
                                                     sphere->Radius,
                                                     rigidbody.MotionType,
                                                     rigidbody.Layer );
    } else if( const auto* box = entity.tryGetComponent< BoxShapeComponent >() ) {
        bodyId = physicsSystem.createBoxCollider( transform.Translation,
                                                  transform.Rotation,
                                                  box->HalfExtents,
                                                  rigidbody.MotionType,
                                                  rigidbody.Layer );
    } else if( const auto* capsule = entity.tryGetComponent< CapsuleShapeComponent >() ) {
        bodyId = physicsSystem.createCapsuleCollider( transform.Translation,
                                                      transform.Rotation,
                                                      capsule->Radius,
                                                      capsule->HalfHeight,
                                                      rigidbody.MotionType,
                                                      rigidbody.Layer );
    }
    ONYX_ASSERT( bodyId != onyx::physics::BodyId::Invalid );

    entityCommandBuffer.addComponent< components::BodyId >( entity, bodyId );
    entityCommandBuffer.removeComponent< InitPhysics >( entity );
}

} // namespace stream_in

namespace simulate {
void system( onyx::physics::PhysicsWorld3d& physicsSystem ) {
    physicsSystem.update();
}
} // namespace simulate

namespace post_physics {
using Access = ecs::Access::Read< components::BodyId >::Write< TransformComponent >::With< components::DynamicBody >;

using PhysiscsEntity = Access::AsEntity;

void system( PhysiscsEntity entity, onyx::physics::PhysicsWorld3d& physicsSystem ) {
    auto&& [ physicsBody, transform ] = entity;
    transform.Translation = physicsSystem.getPosition( physicsBody.Id );
}
} // namespace post_physics

namespace debug_draw {
using Access = ecs::Access ::Read< TransformComponent >::
    ReadIfExists< BoxShapeComponent, CapsuleShapeComponent, SphereShapeComponent >::With< components::BodyId >;

using ColliderEntity = Access::AsEntity;

void system( ColliderEntity entity, onyx::graphics::DebugDrawQueue& debugDraw ) {
    auto&& [ transform ] = entity;

    if( const auto* sphere = entity.tryGetComponent< SphereShapeComponent >() ) {
        debugDraw.addWireframeSphere( transform.Translation, sphere->Radius, onyx::colors::Red );
    }

    if( const auto* box = entity.tryGetComponent< BoxShapeComponent >() ) {
        debugDraw.addWireframeBox( transform.Translation,
                                   box->HalfExtents,
                                   transform.Rotation.toMatrix3(),
                                   onyx::colors::Red );
    }

    if( const auto* capsule = entity.tryGetComponent< CapsuleShapeComponent >() ) {
        debugDraw.addWireframeBox( transform.Translation,
                                   { capsule->Radius, capsule->HalfHeight + capsule->Radius, capsule->Radius },
                                   transform.Rotation.toMatrix3(),
                                   onyx::colors::Red );
    }
}

} // namespace debug_draw

void factory( ecs::EntityRegistry& registry, ecs::EntityId entity, RigidBodyComponent&& rigidbody ) {
    switch( rigidbody.MotionType ) {
        using enum onyx::physics::MotionType;
    case Static: {
        addMotionTypeTag< components::StaticBody, components::KinematicBody, components::DynamicBody >( registry,
                                                                                                        entity );
        break;
    }
    case Kinematic: {
        addMotionTypeTag< components::KinematicBody, components::StaticBody, components::DynamicBody >( registry,
                                                                                                        entity );

        break;
    }
    case Dynamic: {
        addMotionTypeTag< components::DynamicBody, components::StaticBody, components::KinematicBody >( registry,
                                                                                                        entity );

        break;
    }
    }

    registry.addComponent< RigidBodyComponent >( entity, rigidbody );
    registry.addComponent< InitPhysics >( entity );
}

void registerSystems( ecs::EcsBuilder& ecsBuilder ) {
    ecsBuilder.RegisterSystem( stream_in::system );

    ecsBuilder.RegisterSystem( simulate::system );
    ecsBuilder.RegisterSystem( post_physics::system );

    ecsBuilder.RegisterSystem( debug_draw::system );

    ecsBuilder.RegisterComponent< RigidBodyComponent >( factory );

    ecsBuilder.RegisterComponent< BoxShapeComponent >();
    ecsBuilder.RegisterComponent< CapsuleShapeComponent >();
    ecsBuilder.RegisterComponent< SphereShapeComponent >();
}
} // namespace onyx::game_core::physics
