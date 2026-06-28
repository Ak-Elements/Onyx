#include <onyx/physics/jolt/joltphysicssystem.h>

#include <onyx/physics/character.h>
#include <onyx/physics/characterid.h>
#include <onyx/physics/charactermode.h>
#include <onyx/physics/jolt/joltsdfshape.h>
#include <onyx/physics/motiontype.h>
#include <onyx/physics/physicsworld3d.h>

// The Jolt headers don't include Jolt.h. Always include Jolt.h before including any other Jolt header.
// You can use Jolt.h in your precompiled header to speed up compilation.
#include <Jolt/Jolt.h>

#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include <cstdarg>
namespace onyx::physics::jolt {
namespace {

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
    ONYX_UNUSED( inExpression );
    ::onyx::logAssert( inFile, inLine, inMessage );

    // Breakpoint
    return true;
};
#endif // JPH_ENABLE_ASSERTS
#endif
} // namespace

JoltPhysicsSystem::JoltPhysicsSystem() {
    // Register allocation hook.
    JPH::RegisterDefaultAllocator();

    // Install trace and assert callbacks
    JPH::Trace = TraceImpl;
#if ONYX_ASSERT_ENABLED
    JPH_IF_ENABLE_ASSERTS( JPH::AssertFailed = AssertFailedImpl; )
#endif
    // Create a factory, this class is responsible for creating instances of classes based on their name or hash and is
    // mainly used for deserialization of saved data.
    JPH::Factory::sInstance = new JPH::Factory();

    JPH::RegisterTypes();

    SdfShape::registerCollider();

    m_temporaryAllocator = makeUnique< JPH::TempAllocatorImpl >( 10 * 1024 * 1024 );

    m_jobSystem = new JPH::JobSystemThreadPool( JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, 4 - 1 );
}

JoltPhysicsSystem::~JoltPhysicsSystem() {
    JPH::UnregisterTypes();

    // Destroy the factory
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}

PhysicsWorld3d JoltPhysicsSystem::createPhysicsWorld3d() {
    return { *m_temporaryAllocator, *m_jobSystem };
}

} // namespace onyx::physics::jolt
