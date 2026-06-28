#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>

#include <onyx/physics/broadphaselayers.h>
#include <onyx/physics/collisionlayers.h>

namespace onyx::physics::jolt {
/// Class that determines if two object layers can collide
class JoltObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
  public:
    [[nodiscard]] bool ShouldCollide( JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2 ) const override {
        switch( enums::toEnum< CollisionLayer >( inObject1 ) ) {
        case CollisionLayer::Static:
            return enums::toEnum< CollisionLayer >( inObject2 ) ==
                   CollisionLayer::Dynamic; // Non moving only collides with moving
        case CollisionLayer::Dynamic:
            return true; // Moving collides with everything
        case CollisionLayer::Player:
            return true; // Moving collides with everything
        default:
            JPH_ASSERT( false );
            return false;
        }
    }
};

/// Class that determines if an object layer can collide with a broadphase layer
class JoltObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter {
  public:
    [[nodiscard]] bool ShouldCollide( JPH::ObjectLayer objectLayer,
                                      JPH::BroadPhaseLayer broadPhaseLayer ) const override {
        switch( enums::toEnum< CollisionLayer >( objectLayer ) ) {
        case CollisionLayer::Static: {
            const BroadPhaseLayer layer = enums::toEnum< BroadPhaseLayer >( broadPhaseLayer.GetValue() );
            return layer == BroadPhaseLayer::DYNAMIC;
        }
        case CollisionLayer::Dynamic:
            return true;
        case CollisionLayer::Player:
            return true;
        default:
            JPH_ASSERT( false );
            return false;
        }
    }
};
} // namespace onyx::physics::jolt
