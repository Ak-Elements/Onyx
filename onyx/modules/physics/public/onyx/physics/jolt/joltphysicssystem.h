#pragma once

#include <onyx/physics/physicssystem.h>

#include <onyx/physics/character.h> //TODO: Forward declare only
#include <onyx/physics/jolt/joltbroadphaseinterface.h>
#include <onyx/physics/jolt/joltcollisionfilter.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>

namespace JPH {
class PhysicsSystem;
class TempAllocator;
class JobSystemThreadPool;
class DefaultBroadPhaseLayerFilter;
class DefaultObjectLayerFilter;
} // namespace JPH

namespace onyx::physics::jolt {
class JoltBroadPhaseInterface;

class JoltPhysicsSystem : public IPhysicsSystem {
  public:
    JoltPhysicsSystem();
    ~JoltPhysicsSystem() override;

    PhysicsWorld3d createPhysicsWorld3d() override;

    JPH::TempAllocator& getTemporaryAllocator() { return *m_temporaryAllocator; }

  private:
    UniquePtr< JPH::TempAllocator > m_temporaryAllocator;

    // use our own system
    JPH::JobSystemThreadPool* m_jobSystem = nullptr;
};
} // namespace onyx::physics::jolt
