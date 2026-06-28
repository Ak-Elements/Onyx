#include <onyx/physics/jolt/joltcharacter.h>

#include <onyx/physics/collisionlayers.h>
#include <onyx/physics/jolt/physicsworld3d.h>

#include <onyx/physics/movementstate.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

namespace onyx::physics::jolt {

Character::Character() = default;
Character::Character( PhysicsWorld3d& world, const Vector3f32& position, const Rotor3f32& rotation, float32 radius ) {
    JPH::CharacterVirtualSettings settings{};

    m_shape = makeUnique< JPH::SphereShape >( radius );
    settings.mShape = m_shape.get();
    settings.mShapeOffset = JPH::Vec3( 0.0f, radius, 0.0f );

    m_character = makeUnique< JPH::CharacterVirtual >( &settings,
                                                       JPH::Vec3( position.X, position.Y, position.Z ),
                                                       JPH::Quat::sIdentity(),
                                                       uint64_t( 0 ),
                                                       &world.getHandle() );
}

Character::Character( PhysicsWorld3d& world,
                      const Vector3f32& position,
                      const Rotor3f32& rotation,
                      float32 halfHeight,
                      float32 radius ) {
    JPH::CharacterVirtualSettings settings{};

    m_shape = makeUnique< JPH::CapsuleShape >( halfHeight, radius );
    settings.mShape = m_shape.get();
    settings.mShapeOffset = JPH::Vec3( 0.0f, ( halfHeight + radius ), 0.0f );

    m_character = makeUnique< JPH::CharacterVirtual >( &settings,
                                                       JPH::Vec3( position.X, position.Y, position.Z ),
                                                       JPH::Quat::sIdentity(),
                                                       uint64_t( 0 ),
                                                       &world.getHandle() );
}

Character::Character( PhysicsWorld3d& world,
                      const Vector3f32& position,
                      const Rotor3f32& rotation,
                      const Vector3f32& halfExtents ) {
    JPH::CharacterVirtualSettings settings{};

    JPH::Vec3 joltHalfExtents( halfExtents.X, halfExtents.Y, halfExtents.Z );
    m_shape = makeUnique< JPH::BoxShape >( joltHalfExtents );
    settings.mShape = m_shape.get();
    settings.mShapeOffset = joltHalfExtents;

    m_character = makeUnique< JPH::CharacterVirtual >( &settings,
                                                       JPH::Vec3( position.X, position.Y, position.Z ),
                                                       JPH::Quat::sIdentity(),
                                                       uint64_t( 0 ),
                                                       &world.getHandle() );
}
Character::~Character() = default;

Character::Character( Character&& other ) noexcept
    : m_character( std::move( other.m_character ) ) {}

Character& Character::operator=( Character&& other ) noexcept {
    m_character = std::move( other.m_character );
    return *this;
}

void Character::update( PhysicsWorld3d& world, DeltaGameTime deltaTime ) {
    // Update the character position
    JPH::CharacterVirtual::ExtendedUpdateSettings settings;

    [[maybe_unused]] JPH::Vec3 oldPosition = m_character->GetPosition();

    m_character->ExtendedUpdate( numericCast< float32 >( 1.0f / 60.0f ),
                                 -m_character->GetUp() * world.getGravity(),
                                 settings,
                                 world.getDefaultBroadPhaseLayerFilter( CollisionLayer::Player ),
                                 world.getDefaultLayerFilter( CollisionLayer::Player ),
                                 {},
                                 {},
                                 world.getTemporaryAllocator() );
}

CharacterId Character::getId() const {
    return enums::toEnum< CharacterId >( m_character->GetID().GetValue() );
}

bool Character::isSupported() const {
    return m_character->IsSupported();
}

MovementState Character::getMovementState() const {
    switch( m_character->GetGroundState() ) {
    case JPH::CharacterBase::EGroundState::OnGround:
        return MovementState::OnGround;
    case JPH::CharacterBase::EGroundState::OnSteepGround:
        return MovementState::Sliding;
    case JPH::CharacterBase::EGroundState::NotSupported:
        return MovementState::Falling;
    case JPH::CharacterBase::EGroundState::InAir:
        return MovementState::InAir;
    }

    return MovementState::Invalid;
}

Vector3f32 Character::getPosition() const {
    const JPH::Vec3& position = m_character->GetPosition();
    return { position.GetX(), position.GetY(), position.GetZ() };
}

Vector3f32 Character::getLinearVelocity() const {
    JPH::Vec3 linearVelocity = m_character->GetLinearVelocity();
    return { linearVelocity.GetX(), linearVelocity.GetY(), linearVelocity.GetZ() };
}

Vector3f32 Character::getGroundVelocity() const {
    JPH::Vec3 groundVelocity = m_character->GetGroundVelocity();
    return { groundVelocity.GetX(), groundVelocity.GetY(), groundVelocity.GetZ() };
}

void Character::setLinearVelocity( const Vector3f32& velocity ) {
    m_character->SetLinearVelocity( { velocity.X, velocity.Y, velocity.Z } );
}

} // namespace onyx::physics::jolt
