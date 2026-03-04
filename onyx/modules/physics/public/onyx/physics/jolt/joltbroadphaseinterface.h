#pragma once

#include <onyx/physics/broadphaselayers.h>
#include <onyx/physics/collisionlayers.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Body/BodyID.h>
namespace Onyx::Physics::Jolt
{
    class JoltBroadPhaseInterface final : public JPH::BroadPhaseLayerInterface
    {
    public:
        JoltBroadPhaseInterface()
        {
            // Create a mapping table from object to broad phase layer
            mObjectToBroadPhase[Enums::ToIntegral(CollisionLayer::STATIC)] = JPH::BroadPhaseLayer(Enums::ToIntegral(BroadPhaseLayer::STATIC));
            mObjectToBroadPhase[Enums::ToIntegral(CollisionLayer::DYNAMIC)] = JPH::BroadPhaseLayer(Enums::ToIntegral( BroadPhaseLayer::DYNAMIC));
        }

        virtual onyxU32 GetNumBroadPhaseLayers() const override
        {
            return Enums::ToIntegral(BroadPhaseLayer::COUNT);
        }

        virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
        {
            JPH_ASSERT(inLayer < Enums::ToIntegral(CollisionLayer::COUNT));
            return mObjectToBroadPhase[inLayer];
        }

    #if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
        {
            switch (Enums::ToEnum<BroadPhaseLayer>(inLayer.GetValue()))
            {
                case BroadPhaseLayer::STATIC: return "STATIC";
                case BroadPhaseLayer::DYNAMIC:	return "DYNAMIC";
                default: JPH_ASSERT(false); return "INVALID";
            }
        }
    #endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

    private:
        JPH::BroadPhaseLayer mObjectToBroadPhase[Enums::ToIntegral(CollisionLayer::COUNT)];
    };
}