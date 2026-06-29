#include <onyx/gamecore/systems/charactersystem.h>

#include <onyx/colors/base.h>
#include <onyx/entity/ecsbuilder.h>
#include <onyx/entity/entitycommandbuffer.h>
#include <onyx/gamecore/components/physics/characterbodycomponent.gen.h>
#include <onyx/gamecore/components/shapes/boxshapecomponent.gen.h>
#include <onyx/gamecore/components/shapes/capsuleshapecomponent.gen.h>
#include <onyx/gamecore/components/shapes/sphereshapecomponent.gen.h>
#include <onyx/gamecore/components/transformcomponent.gen.h>
#include <onyx/gamecore/gamecore.h>
#include <onyx/gamecore/systems/movementsystem.h>
#include <onyx/gamecore/systems/physicssystem.h>
#include <onyx/physics/character.h>
#include <onyx/physics/characterid.h>
#include <onyx/physics/collisionlayers.h>
#include <onyx/physics/motiontype.h>
#include <onyx/physics/movementstate.h>
#include <onyx/physics/physicsworld3d.h>

namespace onyx::game_core::character_system {
struct StreamIn {};

namespace stream_in {

// clang-format off
using CharacterControllerAccess = ecs::Access
    ::Read< TransformComponent, CharacterBodyComponent >
    ::ReadIfExists< physics::components::CharacterId >
    ::ReadIfExists< BoxShapeComponent, CapsuleShapeComponent, SphereShapeComponent >
    ::With< StreamIn >;
// clang-format on

using CharacterEntity = CharacterControllerAccess::AsEntity;

void system( CharacterEntity entity,
             onyx::physics::PhysicsWorld3d& physicsSystem,
             ecs::EntityCommandBuffer entityCommandBuffer ) {
    auto&& [ transform, characterController ] = entity;

    auto characterId = entity.tryGetComponent< physics::components::CharacterId >();
    if( characterId ) {
        physicsSystem.removeCharacter( characterId->Id );
    }
    onyx::physics::CharacterId id{ onyx::physics::CharacterId::Invalid };
    if( const auto* sphere = entity.tryGetComponent< SphereShapeComponent >() ) {
        id = physicsSystem.createCharacterCollider( transform.Translation,
                                                    transform.Rotation,
                                                    sphere->Radius,
                                                    characterController.Mode,
                                                    onyx::physics::CollisionLayer::Player );
    } else if( const auto* box = entity.tryGetComponent< BoxShapeComponent >() ) {
        id = physicsSystem.createCharacterCollider( transform.Translation,
                                                    transform.Rotation,
                                                    box->HalfExtents,
                                                    characterController.Mode,
                                                    onyx::physics::CollisionLayer::Player );
    } else if( const auto* capsule = entity.tryGetComponent< CapsuleShapeComponent >() ) {
        id = physicsSystem.createCharacterCollider( transform.Translation,
                                                    transform.Rotation,
                                                    capsule->Radius,
                                                    capsule->HalfHeight,
                                                    characterController.Mode,
                                                    onyx::physics::CollisionLayer::Player );
    } else {
        return;
    }

    entityCommandBuffer.addComponent< physics::components::CharacterId >( entity, id );
    entityCommandBuffer.removeComponent< StreamIn >( entity );
};

} // namespace stream_in

namespace update_controller {

// clang-format off
using CharacterControllerAccess = ecs::Access
    ::Read< physics::components::CharacterId, movement_system::components::MovementRequest >
    ::Write< TransformComponent >
    ::With< CharacterBodyComponent, components::Virtual>
    ::Without< StreamIn >;
// clang-format on

using CharacterEntity = CharacterControllerAccess::AsEntity;

void system( CharacterEntity entity, DeltaGameTime deltaTime, onyx::physics::PhysicsWorld3d& physicsWorld ) {
    auto&& [ characterId, movementRequest, transform ] = entity;

    onyx::physics::Character& character = physicsWorld.getCharacter( characterId.Id );
    const onyx::physics::MovementState movementState = character.getMovementState();

    Vector3f32 desiredVelocity;

    if( movementState == onyx::physics::MovementState::OnGround ) {
        desiredVelocity = character.getGroundVelocity();
    } else {
        const Vector3f32 up = Vector3f32::yUnit(); // TODO: get correct up
        desiredVelocity = up * numericCast< float32 >( character.getLinearVelocity().dot( up ) );
    }

    desiredVelocity += Vector3f32( 0.0f, -9.81f, 0.0f ) * ( 1.0f / 60.0f );
    desiredVelocity += movementRequest.RequestedVelocity;
    character.setLinearVelocity( desiredVelocity );
    character.update( physicsWorld, deltaTime );

    transform.Translation = character.getPosition();
}

} // namespace update_controller

namespace debug_draw {

// clang-format off
using Access = ecs::Access
    ::Read< TransformComponent >
    ::ReadIfExists< BoxShapeComponent, CapsuleShapeComponent, SphereShapeComponent >
    ::With< physics::components::CharacterId >;

using CharacterEntity = Access::AsEntity;
// clang-format on

void system( CharacterEntity entity, onyx::graphics::DebugDrawQueue& debugDraw ) {
    auto&& [ transform ] = entity;

    if( const auto* sphere = entity.tryGetComponent< SphereShapeComponent >() ) {
        Vector3f32 offset{ 0.0f, sphere->Radius, 0.0f };
        debugDraw.addWireframeSphere( transform.Translation + offset, sphere->Radius, onyx::colors::Red );
    }

    if( const auto* box = entity.tryGetComponent< BoxShapeComponent >() ) {
        debugDraw.addWireframeBox( transform.Translation + box->HalfExtents,
                                   box->HalfExtents,
                                   transform.Rotation.toMatrix3(),
                                   onyx::colors::Red );
    }

    if( const auto* capsule = entity.tryGetComponent< CapsuleShapeComponent >() ) {
        Vector3f32 offset{ 0.0f, capsule->HalfHeight + capsule->Radius, 0.0f };
        debugDraw.addWireframeBox( transform.Translation + offset,
                                   { capsule->Radius, capsule->HalfHeight + capsule->Radius, capsule->Radius },
                                   transform.Rotation.toMatrix3(),
                                   onyx::colors::Red );
    }
}

} // namespace debug_draw

void factory( ecs::EntityRegistry& registry, ecs::EntityId entity, CharacterBodyComponent&& controller ) {
    registry.addComponent< CharacterBodyComponent >( entity, controller );
    switch( controller.Mode ) {
        using enum onyx::physics::CharacterMode;
    case Simulated: {
        registry.addComponent< character_system::components::Simulated >( entity );
        registry.removeIfExists< character_system::components::Virtual >( entity );
        registry.addComponent< movement_system::components::MovementRequest >( entity );
        break;
    }
    case Virtual: {
        registry.addComponent< character_system::components::Virtual >( entity );
        registry.removeIfExists< character_system::components::Simulated >( entity );
        registry.addComponent< movement_system::components::MovementRequest >( entity );
        break;
    }
    }
    registry.addComponent< StreamIn >( entity );
}

void init( ecs::EcsBuilder& ecsBuilder ) {
    ecsBuilder.registerSystem( stream_in::system );
    ecsBuilder.registerSystem( update_controller::system );
    ecsBuilder.registerSystem( debug_draw::system );

    ecsBuilder.registerComponent< CharacterBodyComponent >( factory );
}
} // namespace onyx::game_core::character_system
