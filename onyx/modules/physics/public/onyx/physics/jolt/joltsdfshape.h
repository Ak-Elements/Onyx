#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

namespace onyx::physics::jolt {
class SdfShape : public JPH::Shape {
  public:
    static void registerCollider();

    using SdfFunction = Vector4f32 ( * )( Vector3f32 position );
    SdfShape();
    SdfShape( SdfFunction function );

    JPH::AABox GetLocalBounds() const override;

    JPH::MassProperties GetMassProperties() const override;

    float GetInnerRadius() const override;

    const JPH::PhysicsMaterial* GetMaterial( const JPH::SubShapeID& subShapeID ) const override;
    uint GetSubShapeIDBitsRecursive() const override;

    JPH::Vec3 GetSurfaceNormal( const JPH::SubShapeID& subShapeId, JPH::Vec3Arg localPos ) const override;

#ifdef JPH_DEBUG_RENDERER // Not using JPH_IF_DEBUG_RENDERER for Doxygen
    void GetSubmergedVolume( JPH::Mat44Arg centerOfMassTransform,
                             JPH::Vec3Arg scale,
                             const JPH::Plane& surface,
                             float& outTotalVolume,
                             float& outSubmergedVolume,
                             JPH::Vec3& outCenterOfBuoyancy,
                             JPH::RVec3Arg baseOffset ) const override;

    void Draw( JPH::DebugRenderer* renderer,
               JPH::RMat44Arg centerOfMassTransform,
               JPH::Vec3Arg scale,
               JPH::ColorArg color,
               bool useMaterialColors,
               bool drawWireframe ) const override;
#else
    void GetSubmergedVolume( JPH::Mat44Arg centerOfMassTransform,
                             JPH::Vec3Arg scale,
                             const JPH::Plane& surface,
                             float& outTotalVolume,
                             float& outSubmergedVolume,
                             JPH::Vec3& outCenterOfBuoyancy ) const override;

#endif

    Vector4f32 evaluateSdf( Vector3f32 position ) const;

    Stats GetStats() const override { return { sizeof( *this ), 0 }; }

    bool CastRay( const JPH::RayCast& ray,
                  const JPH::SubShapeIDCreator& subShapeIdCreator,
                  JPH::RayCastResult& outHit ) const override;

    void CastRay( const JPH::RayCast& ray,
                  const JPH::RayCastSettings& rayCastSettings,
                  const JPH::SubShapeIDCreator& subShapeIDCreator,
                  JPH::CastRayCollector& outCollector,
                  const JPH::ShapeFilter& shapeFilter ) const override;

    void CollidePoint( JPH::Vec3Arg point,
                       const JPH::SubShapeIDCreator& subShapeIDCreator,
                       JPH::CollidePointCollector& outCollector,
                       const JPH::ShapeFilter& shapeFilter ) const override;

    void CollideSoftBodyVertices( JPH::Mat44Arg centerOfMassTransform,
                                  JPH::Vec3Arg scale,
                                  const JPH::CollideSoftBodyVertexIterator& vertices,
                                  uint numVertices,
                                  int collidingShapeIndex ) const override;

    void GetTrianglesStart( GetTrianglesContext& outContext,
                            const JPH::AABox& box,
                            JPH::Vec3Arg positionCOM,
                            JPH::QuatArg rotation,
                            JPH::Vec3Arg scale ) const override;

    int GetTrianglesNext( GetTrianglesContext& outContext,
                          int maxTrianglesRequested,
                          JPH::Float3* outTriangleVertices,
                          const JPH::PhysicsMaterial** outMaterials ) const override;

    float32 GetVolume() const override;

  private:
    SdfFunction m_sdf;
};
} // namespace onyx::physics::jolt
