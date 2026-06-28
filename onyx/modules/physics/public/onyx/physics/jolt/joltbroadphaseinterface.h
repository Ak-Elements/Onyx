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
        m_objectToBroadPhase[ enums::toIntegral( CollisionLayer::Static ) ] = JPH::BroadPhaseLayer(
            enums::toIntegral( BroadPhaseLayer::STATIC ) );
        m_objectToBroadPhase[ enums::toIntegral( CollisionLayer::Dynamic ) ] = JPH::BroadPhaseLayer(
            enums::toIntegral( BroadPhaseLayer::DYNAMIC ) );
        m_objectToBroadPhase[ enums::toIntegral( CollisionLayer::Player ) ] = JPH::BroadPhaseLayer(
            enums::toIntegral( BroadPhaseLayer::DYNAMIC ) );
    }

    [[nodiscard]] uint32_t GetNumBroadPhaseLayers() const override {
        return enums::toIntegral( BroadPhaseLayer::COUNT );
    }

    [[nodiscard]] JPH::BroadPhaseLayer GetBroadPhaseLayer( JPH::ObjectLayer inLayer ) const override {
        JPH_ASSERT( inLayer < enums::toIntegral( CollisionLayer::Count ) );
        return m_objectToBroadPhase[ inLayer ];
    }

#if defined( JPH_EXTERNAL_PROFILE ) || defined( JPH_PROFILE_ENABLED )
    [[nodiscard]] const char* GetBroadPhaseLayerName( JPH::BroadPhaseLayer inLayer ) const override {
        switch( enums::toEnum< BroadPhaseLayer >( inLayer.GetValue() ) ) {
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
    JPH::BroadPhaseLayer m_objectToBroadPhase[ enums::toIntegral( CollisionLayer::Count ) ];
};
} // namespace onyx::physics::jolt
