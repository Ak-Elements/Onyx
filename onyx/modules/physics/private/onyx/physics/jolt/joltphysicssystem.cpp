#include <onyx/physics/jolt/joltphysicssystem.h>

#include <onyx/physics/motiontype.h>

// The Jolt headers don't include Jolt.h. Always include Jolt.h before including any other Jolt header.
// You can use Jolt.h in your precompiled header to speed up compilation.
#include <Jolt/Jolt.h>

#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include <cstdarg>
namespace onyx::physics::jolt {
namespace {
JPH::EMotionType ToMotion( MotionType type ) {
    switch ( type ) {
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

void TraceImpl( const char* inFMT, ... ) {
    // Format the message
    va_list list;
    va_start( list, inFMT );
    char buffer[ 1024 ];
    vsnprintf( buffer, sizeof( buffer ), inFMT, list );
    va_end( list );

    ONYX_LOG_INFO( "{}", buffer );
    // Print to the TTY
    // cout << buffer << endl;
}

#if ONYX_ASSERT_ENABLED
#ifdef JPH_ENABLE_ASSERTS
// Callback for asserts, connect this to your own assert handler if you have one
static bool AssertFailedImpl( const char* inExpression, const char* inMessage, const char* inFile, uint32_t inLine ) {
    ONYX_UNUSED(inExpression);
    ::onyx::logAssert( inFile, inLine, inMessage );

    // Breakpoint
    return true;
};
#endif // JPH_ENABLE_ASSERTS
#endif

// This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an
// error. Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
const uint32_t cMaxBodies = 1024;

// This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the
// default settings.
const uint32_t cNumBodyMutexes = 0;

// This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
// body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this
// buffer too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly
// less efficient. Note: This value is low because this is a simple test. For a real project use something in the order
// of 65536.
const uint32_t cMaxBodyPairs = 1024;

// This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected
// than this number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
// Note: This value is low because this is a simple test. For a real project use something in the order of 10240.
const uint32_t cMaxContactConstraints = 1024;

// JPH::BPLayerInterfaceImpl broad_phase_layer_interface;
// JPH::ObjectVsBroadPhaseLayerFilterImpl object_vs_broadphase_layer_filter;
// JPH::BodyVectorObjectLayerPairFilterImpl object_vs_object_layer_filter;
} // namespace

JoltPhysicsSystem::JoltPhysicsSystem() {
    // Register allocation hook. In this example we'll just let Jolt use malloc / free but you can override these if you
    // want (see Memory.h). This needs to be done before any other Jolt function is called.
    JPH::RegisterDefaultAllocator();

    // Install trace and assert callbacks
    JPH::Trace = TraceImpl;
#if ONYX_ASSERT_ENABLED
    JPH_IF_ENABLE_ASSERTS( JPH::AssertFailed = AssertFailedImpl; )
#endif
    // Create a factory, this class is responsible for creating instances of classes based on their name or hash and is
    // mainly used for deserialization of saved data. It is not directly used in this example but still required.
    JPH::Factory::sInstance = new JPH::Factory();

    JPH::RegisterTypes();

    m_Allocator = makeUnique< JPH::TempAllocatorImpl >( 10 * 1024 * 1024 );

    job_system = new JPH::JobSystemThreadPool( JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, 4 - 1 );

    m_System.Init( cMaxBodies,
                   cNumBodyMutexes,
                   cMaxBodyPairs,
                   cMaxContactConstraints,
                   m_BroadPhase,
                   m_ObjectToBroadhPhaseFilter,
                   m_ObjectToObjectFilter );

    // Activation of a body
    // physics_system.SetBodyActivationListener(&body_activation_listener);

    // contact listener
    // physics_system.SetContactListener(&contact_listener);
}

JoltPhysicsSystem::~JoltPhysicsSystem() {
    JPH::UnregisterTypes();

    // Destroy the factory
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}

void JoltPhysicsSystem::Update() {
    const float32 cDeltaTime = 1.0f / 60.0f;

    // If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the
    // simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
    const int32_t cCollisionSteps = 1;

    // Step the world
    m_System.Update( cDeltaTime, cCollisionSteps, m_Allocator.get(), job_system );
}

Vector3f32 JoltPhysicsSystem::GetPosition( PhysicsBodyId id ) const {
    const JPH::BodyInterface& interface = m_System.GetBodyInterface();

    JPH::RVec3 worldPos;
    JPH::Quat worldRotation;

    interface.GetPositionAndRotation( JPH::BodyID( enums::toIntegral( id ) ), worldPos, worldRotation );
    return { worldPos.GetX(), worldPos.GetY(), worldPos.GetZ() };
}

PhysicsBodyId JoltPhysicsSystem::CreateSphereCollider( const Vector3f32& position,
                                                       const Rotor3f32& /*rotation*/,
                                                       float32 radius,
                                                       MotionType motion,
                                                       CollisionLayer layer ) {
    JPH::BodyInterface& interface = m_System.GetBodyInterface();

    JPH::EMotionType motionType = ToMotion( motion );

    JPH::BodyCreationSettings bodySettings( new JPH::SphereShape( radius ),
                                            JPH::RVec3( position.X, position.Y, position.Z ),
                                            JPH::Quat::sIdentity(),
                                            motionType,
                                            enums::toIntegral( layer ) );
    bodySettings.mMotionQuality = JPH::EMotionQuality::LinearCast;
    JPH::BodyID bodyId = interface.CreateAndAddBody( bodySettings, JPH::EActivation::Activate );
    return enums::toEnum< PhysicsBodyId >( bodyId.GetIndexAndSequenceNumber() );
}

PhysicsBodyId JoltPhysicsSystem::CreateBoxCollider( const Vector3f32& position,
                                                    const Rotor3f32& /*rotation*/,
                                                    const Vector3f32& halfExtents,
                                                    MotionType motion,
                                                    CollisionLayer layer ) {
    JPH::BodyInterface& interface = m_System.GetBodyInterface();

    JPH::RVec3 jphHalfExents( halfExtents.X, halfExtents.Y, halfExtents.Z );
    JPH::EMotionType motionType = ToMotion( motion );

    JPH::BodyCreationSettings bodySettings( new JPH::BoxShape( jphHalfExents ),
                                            JPH::RVec3( position.X, position.Y, position.Z ),
                                            JPH::Quat::sIdentity(),
                                            motionType,
                                            enums::toIntegral( layer ) );
    JPH::BodyID bodyId = interface.CreateAndAddBody( bodySettings, JPH::EActivation::Activate );
    return enums::toEnum< PhysicsBodyId >( bodyId.GetIndexAndSequenceNumber() );
}

} // namespace onyx::physics::jolt