#include <onyx/physics/jolt/physicsworld3d.h>

#include <onyx/physics/characterid.h>
#include <onyx/physics/charactermode.h>
#include <onyx/physics/jolt/joltbroadphaseinterface.h>
#include <onyx/physics/jolt/joltsdfshape.h>
#include <onyx/physics/motiontype.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/PhysicsSystem.h>

namespace onyx::physics::jolt {

namespace {
JPH::EMotionType ToMotion( MotionType type ) {
    switch( type ) {
        using enum onyx::physics::MotionType;
    case Static:
        return JPH::EMotionType::Static;
    case Kinematic:
        return JPH::EMotionType::Kinematic;
    case Dynamic:
        return JPH::EMotionType::Dynamic;
    }

    ONYX_ASSERT( false, "Invalid motion type" );
    return JPH::EMotionType::Static;
}
// This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an
// error. Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
constexpr uint32_t MaxBodies = 1024;

// This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the
// default settings.
constexpr uint32_t NumBodyMutexes = 0;

// This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
// body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this
// buffer too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly
// less efficient. Note: This value is low because this is a simple test. For a real project use something in the order
// of 65536.
constexpr uint32_t MaxBodyPairs = 1024;

// This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected
// than this number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
// Note: This value is low because this is a simple test. For a real project use something in the order of 10240.
constexpr uint32_t MaxContactConstraints = 1024;

} // namespace

PhysicsWorld3d::PhysicsWorld3d( JPH::TempAllocator& allocator, JPH::JobSystem& jobSystem )
    : m_system( makeUnique< JPH::PhysicsSystem >() )
    , m_broadPhase( makeUnique< JoltBroadPhaseInterface >() )
    , m_objectToBroadPhaseFilter( makeUnique< JPH::ObjectVsBroadPhaseLayerFilter >() )
    , m_objectToObjectFilter( makeUnique< JPH::ObjectLayerPairFilter >() )
    , m_allocator( &allocator )
    , m_jobSystem( &jobSystem ) {
    m_system->Init( MaxBodies,
                    NumBodyMutexes,
                    MaxBodyPairs,
                    MaxContactConstraints,
                    *m_broadPhase,
                    *m_objectToBroadPhaseFilter,
                    *m_objectToObjectFilter );
}

PhysicsWorld3d::~PhysicsWorld3d() = default;

void PhysicsWorld3d::update() {
    const float32 cDeltaTime = 1.0f / 60.0f;

    // If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the
    // simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
    const int32_t cCollisionSteps = 1;

    // Step the world
    m_system->Update( cDeltaTime, cCollisionSteps, m_allocator, m_jobSystem );
}

Vector3f32 PhysicsWorld3d::getPosition( BodyId id ) const {
    const JPH::BodyInterface& interface = m_system->GetBodyInterface();

    JPH::RVec3 worldPos;
    JPH::Quat worldRotation;

    interface.GetPositionAndRotation( JPH::BodyID( enums::toIntegral( id ) ), worldPos, worldRotation );
    return { worldPos.GetX(), worldPos.GetY(), worldPos.GetZ() };
}

Vector3f32 PhysicsWorld3d::getPosition( CharacterId id ) const {
    if( m_characters.empty() )
        return Vector3f32::zero();

    return m_characters[ 0 ].getPosition();
}

Character& PhysicsWorld3d::getCharacter( CharacterId id ) {
    return m_characters[ 0 ];
}

JPH::DefaultBroadPhaseLayerFilter PhysicsWorld3d::getDefaultBroadPhaseLayerFilter( CollisionLayer layer ) const {
    return m_system->GetDefaultBroadPhaseLayerFilter( JPH::ObjectLayer( layer ) );
}

JPH::DefaultObjectLayerFilter PhysicsWorld3d::getDefaultLayerFilter( CollisionLayer layer ) const {
    return m_system->GetDefaultLayerFilter( JPH::ObjectLayer( layer ) );
}

void PhysicsWorld3d::removeCollider( BodyId id ) {
    JPH::BodyInterface& interface = m_system->GetBodyInterface();
    interface.RemoveBody( JPH::BodyID( enums::toIntegral( id ) ) );
}

float32 PhysicsWorld3d::getGravity() const {
    return m_system->GetGravity().Length();
}

void PhysicsWorld3d::removeCharacter( CharacterId id ) {
    std::erase_if( m_characters, [ & ]( const Character& character ) { return character.getId() == id; } );
}

BodyId PhysicsWorld3d::createSphereCollider( const Vector3f32& position,
                                             const Rotor3f32& /*rotation*/,
                                             float32 radius,
                                             MotionType motion,
                                             CollisionLayer layer ) {
    JPH::BodyInterface& interface = m_system->GetBodyInterface();

    JPH::EMotionType motionType = ToMotion( motion );

    JPH::BodyCreationSettings bodySettings( new JPH::SphereShape( radius ),
                                            JPH::RVec3( position.X, position.Y, position.Z ),
                                            JPH::Quat::sIdentity(),
                                            motionType,
                                            enums::toIntegral( layer ) );
    bodySettings.mMotionQuality = JPH::EMotionQuality::LinearCast;
    JPH::BodyID bodyId = interface.CreateAndAddBody( bodySettings, JPH::EActivation::Activate );
    return enums::toEnum< BodyId >( bodyId.GetIndexAndSequenceNumber() );
}

BodyId PhysicsWorld3d::createBoxCollider( const Vector3f32& position,
                                          const Rotor3f32& /*rotation*/,
                                          const Vector3f32& halfExtents,
                                          MotionType motion,
                                          CollisionLayer layer ) {
    JPH::BodyInterface& interface = m_system->GetBodyInterface();

    JPH::RVec3 jphHalfExents( halfExtents.X, halfExtents.Y, halfExtents.Z );
    JPH::EMotionType motionType = ToMotion( motion );

    JPH::BodyCreationSettings bodySettings( new JPH::BoxShape( jphHalfExents ),
                                            JPH::RVec3( position.X, position.Y, position.Z ),
                                            JPH::Quat::sIdentity(),
                                            motionType,
                                            enums::toIntegral( layer ) );
    JPH::BodyID bodyId = interface.CreateAndAddBody( bodySettings, JPH::EActivation::Activate );
    return enums::toEnum< BodyId >( bodyId.GetIndexAndSequenceNumber() );
}

BodyId PhysicsWorld3d::createCapsuleCollider( const Vector3f32& position,
                                              const Rotor3f32& rotation,
                                              float32 radius,
                                              float32 halfHeight,
                                              MotionType motion,
                                              CollisionLayer layer ) {
    JPH::BodyInterface& interface = m_system->GetBodyInterface();

    JPH::EMotionType motionType = ToMotion( motion );

    JPH::BodyCreationSettings bodySettings( new JPH::CapsuleShape( halfHeight, radius ),
                                            JPH::RVec3( position.X, position.Y, position.Z ),
                                            JPH::Quat::sIdentity(),
                                            motionType,
                                            enums::toIntegral( layer ) );
    JPH::BodyID bodyId = interface.CreateAndAddBody( bodySettings, JPH::EActivation::Activate );
    return enums::toEnum< BodyId >( bodyId.GetIndexAndSequenceNumber() );
}

CharacterId PhysicsWorld3d::createCharacterCollider( const Vector3f32& position,
                                                     const Rotor3f32& rotation,
                                                     float32 radius,
                                                     CharacterMode mode,
                                                     CollisionLayer layer ) {
    if( mode == CharacterMode::Simulated ) {
        // TODO: Add simulated character
        return CharacterId::Invalid;
    } else {
        Character& newCharacter = m_characters.emplace_back( *this, position, rotation, radius );
        return newCharacter.getId();
    }
}

CharacterId PhysicsWorld3d::createCharacterCollider( const Vector3f32& position,
                                                     const Rotor3f32& rotation,
                                                     float32 radius,
                                                     float32 halfHeight,
                                                     CharacterMode mode,
                                                     CollisionLayer layer ) {
    if( mode == CharacterMode::Simulated ) {
        // TODO: Add simulated character
        return CharacterId::Invalid;
    } else {
        Character& newCharacter = m_characters.emplace_back( *this, position, rotation, halfHeight, radius );
        return newCharacter.getId();
    }
}

CharacterId PhysicsWorld3d::createCharacterCollider( const Vector3f32& position,
                                                     const Rotor3f32& rotation,
                                                     const Vector3f32& halfExtents,
                                                     CharacterMode mode,
                                                     CollisionLayer layer ) {
    if( mode == CharacterMode::Simulated ) {
        // TODO: Add simulated character
        return CharacterId::Invalid;
    } else {
        Character& newCharacter = m_characters.emplace_back( *this, position, rotation, halfExtents );
        return newCharacter.getId();
    }
}
BodyId PhysicsWorld3d::createSdfCollider( const Vector3f32& position,
                                          const Rotor3f32& rotation,
                                          SdfFunction function,
                                          MotionType motion,
                                          CollisionLayer layer ) {
    JPH::BodyInterface& interface = m_system->GetBodyInterface();

    JPH::EMotionType motionType = ToMotion( motion );

    JPH::BodyCreationSettings bodySettings( new jolt::SdfShape( function ),
                                            JPH::RVec3( position.X, position.Y, position.Z ),
                                            JPH::Quat::sIdentity(),
                                            motionType,
                                            enums::toIntegral( layer ) );

    JPH::BodyID bodyId = interface.CreateAndAddBody( bodySettings, JPH::EActivation::Activate );
    return enums::toEnum< BodyId >( bodyId.GetIndexAndSequenceNumber() );
}

} // namespace onyx::physics::jolt
