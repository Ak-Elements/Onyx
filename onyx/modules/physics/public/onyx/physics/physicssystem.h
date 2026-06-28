#pragma once

#include <onyx/engine/enginesystem.h>
#include <onyx/physics/characterfwd.h>
#include <onyx/physics/physicsworld3dfwd.h>

namespace onyx::physics {

class IPhysicsSystem {
  public:
    virtual ~IPhysicsSystem() = default;
    virtual PhysicsWorld3d createPhysicsWorld3d() = 0;
};

class PhysicsSystem : public IEngineSystem {
  public:
    static constexpr StringId32 TypeId = "onyx::physics::PhysicsSystem";
    [[nodiscard]] StringId32 getTypeId() const override { return TypeId; }

    PhysicsSystem();
    ~PhysicsSystem();

    PhysicsWorld3d createPhysicsWorld3d();

  private:
    UniquePtr< IPhysicsSystem > m_system;
};

} // namespace onyx::physics
