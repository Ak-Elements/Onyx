#pragma once

#include <onyx/physics/character.h>

namespace JPH { // NOLINT
class PhysicsSystem;
class TempAllocator;
class JobSystem;
class DefaultBroadPhaseLayerFilter;
class DefaultObjectLayerFilter;
class ObjectLayerPairFilter;
class ObjectVsBroadPhaseLayerFilter;
} // namespace JPH

namespace onyx::physics {

enum class MotionType : uint8_t;
enum class CollisionLayer : uint16_t;
enum class BodyId : uint32_t;
enum class CharacterId : uint32_t;
enum class CharacterMode : uint8_t;

namespace jolt {
class JoltBroadPhaseInterface;

class PhysicsWorld3d {
  public:
    PhysicsWorld3d( JPH::TempAllocator& allocator, JPH::JobSystem& jobSystem );
    ~PhysicsWorld3d();
    void update();

    [[nodiscard]] Vector3f32 getPosition( BodyId id ) const;
    [[nodiscard]] Vector3f32 getPosition( CharacterId id ) const;

    Character& getCharacter( CharacterId id );

    [[nodiscard]] float32 getGravity() const;

    void removeCollider( BodyId id );
    void removeCharacter( CharacterId id );

    BodyId createSphereCollider( const Vector3f32& position,
                                 const Rotor3f32& rotation,
                                 float32 radius,
                                 MotionType motion,
                                 CollisionLayer layer );

    BodyId createBoxCollider( const Vector3f32& position,
                              const Rotor3f32& rotation,
                              const Vector3f32& halfExtents,
                              MotionType motion,
                              CollisionLayer layer );

    BodyId createCapsuleCollider( const Vector3f32& position,
                                  const Rotor3f32& rotation,
                                  float32 radius,
                                  float32 halfHeight,
                                  MotionType motion,
                                  CollisionLayer layer );

    // sphere character controller
    CharacterId createCharacterCollider( const Vector3f32& position,
                                         const Rotor3f32& rotation,
                                         float32 radius,
                                         CharacterMode mode,
                                         CollisionLayer layer );
    // box character controller
    CharacterId createCharacterCollider( const Vector3f32& position,
                                         const Rotor3f32& rotation,
                                         const Vector3f32& halfExtents,
                                         CharacterMode mode,
                                         CollisionLayer layer );
    // capsule character controller
    CharacterId createCharacterCollider( const Vector3f32& position,
                                         const Rotor3f32& rotation,
                                         float32 radius,
                                         float32 halfHeight,
                                         CharacterMode mode,
                                         CollisionLayer layer );

    using SdfFunction = Vector4f32 ( * )( Vector3f32 position );
    BodyId createSdfCollider( const Vector3f32& position,
                              const Rotor3f32& rotation,
                              SdfFunction function,
                              MotionType motion,
                              CollisionLayer layer );

    [[nodiscard]] JPH::DefaultBroadPhaseLayerFilter getDefaultBroadPhaseLayerFilter( CollisionLayer layer ) const;
    [[nodiscard]] JPH::DefaultObjectLayerFilter getDefaultLayerFilter( CollisionLayer layer ) const;
    [[nodiscard]] JPH::TempAllocator& getTemporaryAllocator() const { return *m_allocator; }

    [[nodiscard]] JPH::PhysicsSystem& getHandle() { return *m_system; }

  private:
    UniquePtr< JPH::PhysicsSystem > m_system;

    DynamicArray< Character > m_characters;

    UniquePtr< JoltBroadPhaseInterface > m_broadPhase;
    UniquePtr< JPH::ObjectVsBroadPhaseLayerFilter > m_objectToBroadPhaseFilter;
    UniquePtr< JPH::ObjectLayerPairFilter > m_objectToObjectFilter;

    JPH::TempAllocator* m_allocator;
    JPH::JobSystem* m_jobSystem;
};

} // namespace jolt

} // namespace onyx::physics
