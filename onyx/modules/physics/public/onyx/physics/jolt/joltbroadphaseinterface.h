#pragma once

#include <onyx/physics/broadphaselayers.h>
#include <onyx/physics/collisionlayers.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
namespace onyx::physics::jolt {
class JoltBroadPhaseInterface final : public JPH::BroadPhaseLayerInterface {
  public:
    JoltBroadPhaseInterface() {
        // Create a mapping table from object to broad phase layer
        mObjectToBroadPhase[ enums::toIntegral( CollisionLayer::STATIC ) ] = JPH::BroadPhaseLayer(
            enums::toIntegral( BroadPhaseLayer::STATIC ) );
        mObjectToBroadPhase[ enums::toIntegral( CollisionLayer::DYNAMIC ) ] = JPH::BroadPhaseLayer(
            enums::toIntegral( BroadPhaseLayer::DYNAMIC ) );
    }

    virtual uint32_t GetNumBroadPhaseLayers() const override { return enums::toIntegral( BroadPhaseLayer::COUNT ); }

    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer( JPH::ObjectLayer inLayer ) const override {
        JPH_ASSERT( inLayer < enums::toIntegral( CollisionLayer::COUNT ) );
        return mObjectToBroadPhase[ inLayer ];
    }

#if defined( JPH_EXTERNAL_PROFILE ) || defined( JPH_PROFILE_ENABLED )
    virtual const char* GetBroadPhaseLayerName( JPH::BroadPhaseLayer inLayer ) const override {
        switch ( enums::toEnum< BroadPhaseLayer >( inLayer.GetValue() ) ) {
        case BroadPhaseLayer::STATIC:
            return "STATIC";
        case BroadPhaseLayer::DYNAMIC:
            return "DYNAMIC";
        default:
            JPH_ASSERT( false );
            return "INVALID";
        }
    }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

  private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[ enums::toIntegral( CollisionLayer::COUNT ) ];
};
} // namespace onyx::physics::jolt