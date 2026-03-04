#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

#include <onyx/physics/collisionlayers.h>
#include <onyx/physics/broadphaselayers.h>

namespace Onyx::Physics::Jolt
{
    /// Class that determines if two object layers can collide
    class JoltObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
    {
    public:
        bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
        {
            switch (Enums::ToEnum<CollisionLayer>(inObject1))
            {
            case CollisionLayer::STATIC:
                return Enums::ToEnum<CollisionLayer>(inObject2) == CollisionLayer::DYNAMIC; // Non moving only collides with moving
            case CollisionLayer::DYNAMIC:
                return true; // Moving collides with everything
            default:
                JPH_ASSERT(false);
                return false;
            }
        }
    };

    /// Class that determines if an object layer can collide with a broadphase layer
    class JoltObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
    {
    public:
        bool ShouldCollide(JPH::ObjectLayer objectLayer, JPH::BroadPhaseLayer broadPhaseLayer) const override
        {
            switch (Enums::ToEnum<CollisionLayer>(objectLayer))
            {
            case CollisionLayer::STATIC:
                return Enums::ToEnum<BroadPhaseLayer>(broadPhaseLayer.GetValue()) == BroadPhaseLayer::DYNAMIC;
            case CollisionLayer::DYNAMIC:
                return true;
            default:
                JPH_ASSERT(false);
                return false;
            }
        }
    };
}