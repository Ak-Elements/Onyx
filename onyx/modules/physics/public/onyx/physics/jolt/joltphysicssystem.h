#pragma once

#include <onyx/physics/physicssystem.h>

#include <onyx/physics/jolt/joltbroadphaseinterface.h>
#include <onyx/physics/jolt/joltcollisionfilter.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>


namespace JPH
{
    class PhysicsSystem;
    class TempAllocator;
    class JobSystemThreadPool;
}

namespace onyx::physics::jolt
{
    class JoltBroadPhaseInterface;

    class JoltPhysicsSystem : public IPhysicsSystem
    {
    public:
        JoltPhysicsSystem();
        ~JoltPhysicsSystem();

        void Update() override;

        Vector3f32 GetPosition(PhysicsBodyId id) const override;

        PhysicsBodyId CreateSphereCollider(const Vector3f32& position, const Rotor3f32& rotation, onyxF32 radius, MotionType motion, CollisionLayer layer) override;       
        PhysicsBodyId CreateBoxCollider(const Vector3f32& position, const Rotor3f32& rotation, const Vector3f32& halfExtents, MotionType motion, CollisionLayer layer) override;       

    private:
        JPH::PhysicsSystem m_System;
        UniquePtr<JPH::TempAllocator> m_Allocator;
        
        JoltBroadPhaseInterface m_BroadPhase;
        JoltObjectLayerPairFilterImpl m_ObjectToObjectFilter;
        JoltObjectVsBroadPhaseLayerFilterImpl m_ObjectToBroadhPhaseFilter;
        
        // use our own system
        JPH::JobSystemThreadPool* job_system = nullptr;
    };
}
