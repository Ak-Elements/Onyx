#include <onyx/physics/physicssystem.h>

#include <onyx/physics/jolt/joltphysicssystem.h>

namespace Onyx::Physics
{
    PhysicsWorld::PhysicsWorld()
    {
        m_System = MakeUnique<Jolt::JoltPhysicsSystem>();
    }

    PhysicsWorld::~PhysicsWorld()
    {
    }

    void PhysicsWorld::Update()
    {
        m_System->Update();
    }

    Vector3f32 PhysicsWorld::GetPosition(PhysicsBodyId id)
    {
        return m_System->GetPosition(id);
    }

    PhysicsBodyId PhysicsWorld::CreateSphereCollider(const Vector3f32& position, const Rotor3f32& rotation, onyxF32 radius, MotionType motion, CollisionLayer layer)
    {
        return m_System->CreateSphereCollider(position,rotation, radius, motion, layer);
    }      
    
    PhysicsBodyId PhysicsWorld::CreateBoxCollider(const Vector3f32& position, const Rotor3f32& rotation, const Vector3f32& halfExtents, MotionType motion, CollisionLayer layer)
    {
        return m_System->CreateBoxCollider(position,rotation, halfExtents, motion, layer);
    }   

}