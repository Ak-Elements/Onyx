#include <onyx/gamecore/systems/physicssystem.h>

#include <onyx/colors/base.h>
#include <onyx/entity/entitycommandbuffer.h>
#include <onyx/entity/entitycomponentsystem.h>
#include <onyx/gamecore/components/physics/kinematicbodycomponent.gen.h>
#include <onyx/gamecore/components/physics/rigidbodycomponent.gen.h>
#include <onyx/gamecore/components/physics/staticbodycomponent.gen.h>
#include <onyx/gamecore/components/shapes/boxshapecomponent.gen.h>
#include <onyx/gamecore/components/shapes/capsuleshapecomponent.gen.h>
#include <onyx/gamecore/components/shapes/sphereshapecomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/scene/scene.h>
#include <onyx/graphics/debug/debugdrawqueue.h>
#include <onyx/physics/bodyid.h>
#include <onyx/physics/character.h>
#include <onyx/physics/motiontype.h>
#include <onyx/physics/physicsworld3d.h>

namespace onyx::game_core::physics {

struct InitPhysics {};

namespace stream_in {

template < typename EntityAccessT >
void initPhysicsEntity( ecs::EntityCommandBuffer& commandBuffer,
                        onyx::physics::PhysicsWorld3d& physicsWorld,
                        EntityAccessT entity,
                        const TransformComponent& transform,
                        onyx::physics::CollisionLayer layer,
                        onyx::physics::MotionType motionType ) {
    auto physicsBodyID = entity.template tryGetComponent< components::BodyId >();
    if( physicsBodyID ) {
        physicsWorld.removeCollider( physicsBodyID->Id );
    }

    // TODO: how do we prevent the presence of multiple shape components
    onyx::physics::BodyId bodyId{ onyx::physics::BodyId::Invalid };
    if( const auto* sphere = entity.template tryGetComponent< SphereShapeComponent >() ) {
        bodyId = physicsWorld.createSphereCollider( transform.Translation,
                                                    transform.Rotation,
                                                    sphere->Radius,
                                                    onyx::physics::MotionType::Dynamic,
                                                    layer );
    } else if( const auto* box = entity.template tryGetComponent< BoxShapeComponent >() ) {
        bodyId = physicsWorld.createBoxCollider( transform.Translation,
                                                 transform.Rotation,
                                                 box->HalfExtents,
                                                 onyx::physics::MotionType::Dynamic,
                                                 layer );
    } else if( const auto* capsule = entity.template tryGetComponent< CapsuleShapeComponent >() ) {
        bodyId = physicsWorld.createCapsuleCollider( transform.Translation,
                                                     transform.Rotation,
                                                     capsule->Radius,
                                                     capsule->HalfHeight,
                                                     onyx::physics::MotionType::Dynamic,
                                                     layer );
    }
    ONYX_ASSERT( bodyId != onyx::physics::BodyId::Invalid );

    commandBuffer.addComponent< components::BodyId >( entity, bodyId );
    commandBuffer.removeComponent< InitPhysics >( entity );
}

// clang-format off
using KinematicBodyAccess = ecs::Access
    ::Read< TransformComponent >
    ::ReadIfExists< BoxShapeComponent, CapsuleShapeComponent, SphereShapeComponent >
    ::ReadIfExists< components::BodyId >
    ::With< KinematicBodyComponent, InitPhysics >;
// clang-format on

using KinematicBodyEntity = KinematicBodyAccess::AsEntity;

void streamInKinematicBodies( KinematicBodyEntity entity,
                              onyx::physics::PhysicsWorld3d& physicsWorld,
                              ecs::EntityCommandBuffer entityCommandBuffer ) {
    auto&& [ transform ] = entity;
    initPhysicsEntity( entityCommandBuffer,
                       physicsWorld,
                       entity,
                       transform,
                       onyx::physics::CollisionLayer::Static,
                       onyx::physics::MotionType::Kinematic );
}

// clang-format off
using RigidBodyAccess = ecs::Access
    ::Read< TransformComponent >
    ::Read< RigidBodyComponent >
    ::ReadIfExists< BoxShapeComponent, CapsuleShapeComponent, SphereShapeComponent >
    ::ReadIfExists< components::BodyId >
    ::With< InitPhysics >;
// clang-format on

using RigidBodyEntity = RigidBodyAccess::AsEntity;

void streamInRigidBodies( RigidBodyEntity entity,
                          onyx::physics::PhysicsWorld3d& physicsWorld,
                          ecs::EntityCommandBuffer entityCommandBuffer ) {
    auto&& [ transform, rigidBody ] = entity;
    initPhysicsEntity( entityCommandBuffer,
                       physicsWorld,
                       entity,
                       transform,
                       rigidBody.Layer,
                       onyx::physics::MotionType::Dynamic );
}

// clang-format off
using StaticBodyAccess = ecs::Access
    ::Read< TransformComponent >
    ::ReadIfExists< BoxShapeComponent, CapsuleShapeComponent, SphereShapeComponent >
    ::ReadIfExists< components::BodyId >
    ::With< StaticBodyComponent, InitPhysics >;
// clang-format on

using StaticBodyEntity = StaticBodyAccess::AsEntity;

void streamInStaticBodies( StaticBodyEntity entity,
                           onyx::physics::PhysicsWorld3d& physicsWorld,
                           ecs::EntityCommandBuffer entityCommandBuffer ) {
    auto&& [ transform ] = entity;
    initPhysicsEntity( entityCommandBuffer,
                       physicsWorld,
                       entity,
                       transform,
                       onyx::physics::CollisionLayer::Static,
                       onyx::physics::MotionType::Static );
}
} // namespace stream_in

namespace simulate {
void system( onyx::physics::PhysicsWorld3d& physicsSystem ) {
    physicsSystem.update();
}
} // namespace simulate

namespace post_physics {

// clang-format off
using Access = ecs::Access
    ::Read< components::BodyId >
    ::Write< TransformComponent >
    ::With< RigidBodyComponent >;

using PhysiscsEntity = Access::AsEntity;
// clang-format on

void system( PhysiscsEntity entity, onyx::physics::PhysicsWorld3d& physicsSystem ) {
    auto&& [ physicsBody, transform ] = entity;
    transform.Translation = physicsSystem.getPosition( physicsBody.Id );
}
} // namespace post_physics

namespace debug_draw {

// clang-format off
using Access = ecs::Access
    ::Read< TransformComponent >
    ::ReadIfExists< BoxShapeComponent, CapsuleShapeComponent, SphereShapeComponent >
    ::With< components::BodyId >;

using ColliderEntity = Access::AsEntity;
// clang-format on

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

void factory( ecs::EntityRegistry& registry, ecs::EntityId entity, KinematicBodyComponent&& staticBody ) {
    registry.removeIfExists< StaticBodyComponent >( entity );
    registry.removeIfExists< RigidBodyComponent >( entity );
    registry.addComponent< KinematicBodyComponent >( entity );
    registry.addComponent< InitPhysics >( entity );
}

void factory( ecs::EntityRegistry& registry, ecs::EntityId entity, RigidBodyComponent&& rigidBody ) {
    registry.removeIfExists< KinematicBodyComponent >( entity );
    registry.removeIfExists< StaticBodyComponent >( entity );
    registry.addComponent< RigidBodyComponent >( entity, rigidBody );
    registry.addComponent< InitPhysics >( entity );
}

void factory( ecs::EntityRegistry& registry, ecs::EntityId entity, StaticBodyComponent&& staticBody ) {
    registry.removeIfExists< KinematicBodyComponent >( entity );
    registry.removeIfExists< RigidBodyComponent >( entity );
    registry.addComponent< StaticBodyComponent >( entity );
    registry.addComponent< InitPhysics >( entity );
}

void registerSystems( ecs::EcsBuilder& ecsBuilder ) {
    ecsBuilder.registerSystem( stream_in::streamInKinematicBodies );
    ecsBuilder.registerSystem( stream_in::streamInRigidBodies );
    ecsBuilder.registerSystem( stream_in::streamInStaticBodies );

    ecsBuilder.registerSystem( simulate::system );
    ecsBuilder.registerSystem( post_physics::system );

    ecsBuilder.registerSystem( debug_draw::system );

    ecsBuilder.registerComponent< KinematicBodyComponent >( factory );
    ecsBuilder.registerComponent< RigidBodyComponent >( factory );
    ecsBuilder.registerComponent< StaticBodyComponent >( factory );

    ecsBuilder.registerComponent< BoxShapeComponent >();
    ecsBuilder.registerComponent< CapsuleShapeComponent >();
    ecsBuilder.registerComponent< SphereShapeComponent >();
}
} // namespace onyx::game_core::physics
