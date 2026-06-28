#include <onyx/physics/physicssystem.h>

#include <onyx/physics/jolt/joltphysicssystem.h>
#include <onyx/physics/physicsworld3d.h>

namespace onyx::physics {
PhysicsSystem::PhysicsSystem() {
    m_system = makeUnique< jolt::JoltPhysicsSystem >();
}

PhysicsSystem::~PhysicsSystem() = default;

PhysicsWorld3d PhysicsSystem::createPhysicsWorld3d() {
    return m_system->createPhysicsWorld3d();
}

} // namespace onyx::physics
