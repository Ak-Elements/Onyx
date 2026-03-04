#pragma once

#include <onyx/engine/enginesystem.h>

namespace Onyx::Physics
{
    enum class MotionType : onyxU8;
    enum class CollisionLayer : onyxU16;

    enum class PhysicsBodyId : onyxU32
    {
        Invalid = std::numeric_limits<onyxU32>::max()
    };

    class IPhysicsSystem
    {
    public:
        virtual ~IPhysicsSystem() = default;

        virtual void Update() = 0;

        virtual Vector3f32 GetPosition(PhysicsBodyId id) const = 0;

        virtual PhysicsBodyId CreateSphereCollider(const Vector3f32& position, const Rotor3f32& rotation, onyxF32 radius, MotionType motion, CollisionLayer layer) = 0;       
        virtual PhysicsBodyId CreateBoxCollider(const Vector3f32& position, const Rotor3f32& rotation, const Vector3f32& halfExtents, MotionType motion, CollisionLayer layer) = 0;       

    };

    class PhysicsWorld
    {
    public:
        PhysicsWorld();
        ~PhysicsWorld();

        void Update();

        Vector3f32 GetPosition(PhysicsBodyId id);

        PhysicsBodyId CreateSphereCollider(const Vector3f32& position, const Rotor3f32& rotation, onyxF32 radius, MotionType motion, CollisionLayer layer);       
        PhysicsBodyId CreateBoxCollider(const Vector3f32& position, const Rotor3f32& rotation, const Vector3f32& halfExtents, MotionType motion, CollisionLayer layer);       

    private:
        UniquePtr<IPhysicsSystem> m_System;
    };
}
