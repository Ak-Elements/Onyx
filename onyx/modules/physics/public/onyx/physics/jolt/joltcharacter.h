#pragma once

namespace JPH {
class CharacterVirtual;
class Shape;
}; // namespace JPH

namespace onyx::physics {

enum class CharacterId : uint32_t;
enum class MovementState : uint8_t;

namespace jolt {
class JoltPhysicsSystem;
class PhysicsWorld3d;

class Character {
  public:
    Character();
    Character( PhysicsWorld3d& world, const Vector3f32& position, const Rotor3f32& rotation, float32 radius );

    Character( PhysicsWorld3d& world,
               const Vector3f32& position,
               const Rotor3f32& rotation,
               const Vector3f32& halfExtents );

    Character( PhysicsWorld3d& world,
               const Vector3f32& position,
               const Rotor3f32& rotation,
               float32 halfHeight,
               float32 radius );
    ~Character();

    Character( Character&& other ) noexcept;
    Character& operator=( Character&& other ) noexcept;

    void update( PhysicsWorld3d& world, DeltaGameTime deltaTime );

    [[nodiscard]] CharacterId getId() const;
    [[nodiscard]] bool isSupported() const;
    [[nodiscard]] MovementState getMovementState() const;
    [[nodiscard]] Vector3f32 getLinearVelocity() const;
    [[nodiscard]] Vector3f32 getGroundVelocity() const;
    void setLinearVelocity( const Vector3f32& velocity );

    [[nodiscard]] Vector3f32 getPosition() const;

    JPH::CharacterVirtual& getCharacter() { return *m_character; }

  private:
    UniquePtr< JPH::CharacterVirtual > m_character;
    UniquePtr< JPH::Shape > m_shape;
};

} // namespace jolt

} // namespace onyx::physics
