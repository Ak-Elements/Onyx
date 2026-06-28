#include <onyx/physics/jolt/joltsdfshape.h>

#include <onyx/physics/jolt/joltsdfcollision.h>

#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollidePointResult.h>
#include <Jolt/Physics/Collision/PhysicsMaterial.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

#include <Jolt/Physics/Collision/CollisionDispatch.h>

namespace onyx::physics::jolt {
void SdfShape::registerCollider() {
    JPH::ShapeFunctions& f = JPH::ShapeFunctions::sGet( JPH::EShapeSubType::User1 );
    f.mConstruct = []() -> JPH::Shape* { return new SdfShape; };
    f.mColor = JPH::Color::sGreen;

    // sphere
    JPH::CollisionDispatch::sRegisterCollideShape( JPH::EShapeSubType::User1,
                                                   JPH::EShapeSubType::Sphere,
                                                   sdf::collideSphere );
    JPH::CollisionDispatch::sRegisterCollideShape( JPH::EShapeSubType::Sphere,
                                                   JPH::EShapeSubType::User1,
                                                   JPH::CollisionDispatch::sReversedCollideShape );

    JPH::CollisionDispatch::sRegisterCastShape( JPH::EShapeSubType::Sphere,
                                                JPH::EShapeSubType::User1,
                                                sdf::castSphere );

    // box
    JPH::CollisionDispatch::sRegisterCollideShape( JPH::EShapeSubType::User1,
                                                   JPH::EShapeSubType::Box,
                                                   sdf::collideBox );
    JPH::CollisionDispatch::sRegisterCollideShape( JPH::EShapeSubType::Box,
                                                   JPH::EShapeSubType::User1,
                                                   JPH::CollisionDispatch::sReversedCollideShape );

    // capsule
    JPH::CollisionDispatch::sRegisterCollideShape( JPH::EShapeSubType::User1,
                                                   JPH::EShapeSubType::Capsule,
                                                   sdf::collideCapsule );
    JPH::CollisionDispatch::sRegisterCollideShape( JPH::EShapeSubType::Capsule,
                                                   JPH::EShapeSubType::User1,
                                                   JPH::CollisionDispatch::sReversedCollideShape );

    JPH::CollisionDispatch::sRegisterCastShape( JPH::EShapeSubType::Capsule,
                                                JPH::EShapeSubType::User1,
                                                sdf::castCapsule );
}

SdfShape::SdfShape()
    : JPH::Shape( JPH::EShapeType::User1, JPH::EShapeSubType::User1 )
    , m_sdf( nullptr ) {}

SdfShape::SdfShape( SdfFunction function )
    : JPH::Shape( JPH::EShapeType::User1, JPH::EShapeSubType::User1 )
    , m_sdf( function ) {}

JPH::AABox SdfShape::GetLocalBounds() const {
    return { JPH::Vec3( -32000, -512, -32000 ), JPH::Vec3( 32000, 4000, 32000 ) };
}

JPH::MassProperties SdfShape::GetMassProperties() const {
    return {};
}

uint SdfShape::GetSubShapeIDBitsRecursive() const {
    return 0;
}

float SdfShape::GetInnerRadius() const {
    return 0.0f;
}

const JPH::PhysicsMaterial* SdfShape::GetMaterial( [[maybe_unused]] const JPH::SubShapeID& subShapeId ) const {
    return JPH::PhysicsMaterial::sDefault;
}

JPH::Vec3 SdfShape::GetSurfaceNormal( const JPH::SubShapeID& subShapeId, JPH::Vec3Arg localPos ) const {
    ONYX_UNUSED( subShapeId );
    Vector4f32 sample = m_sdf( { localPos.GetX(), localPos.GetY(), localPos.GetZ() } );
    JPH::Vec3 gradient{ sample.X, sample.Y, sample.Z };
    return gradient;
}

#ifdef JPH_DEBUG_RENDERER
void SdfShape::GetSubmergedVolume( [[maybe_unused]] JPH::Mat44Arg centerOfMassTransform,
                                   [[maybe_unused]] JPH::Vec3Arg scale,
                                   [[maybe_unused]] const JPH::Plane& surface,
                                   [[maybe_unused]] float& outTotalVolume,
                                   [[maybe_unused]] float& outSubmergedVolume,
                                   [[maybe_unused]] JPH::Vec3& outCenterOfBuoyancy,
                                   [[maybe_unused]] JPH::RVec3Arg baseOffset ) const {
#else
void SdfShape::GetSubmergedVolume( [[maybe_unused]] JPH::Mat44Arg centerOfMassTransform,
                                   [[maybe_unused]] JPH::Vec3Arg scale,
                                   [[maybe_unused]] const JPH::Plane& surface,
                                   [[maybe_unused]] float& outTotalVolume,
                                   [[maybe_unused]] float& outSubmergedVolume,
                                   [[maybe_unused]] JPH::Vec3& outCenterOfBuoyancy ) const {
#endif
    outTotalVolume = 0.0f;
    outSubmergedVolume = 0.0f;
    outCenterOfBuoyancy = JPH::Vec3::sZero();
}

#ifdef JPH_DEBUG_RENDERER
void SdfShape::Draw( [[maybe_unused]] JPH::DebugRenderer* renderer,
                     [[maybe_unused]] JPH::RMat44Arg centerOfMassTransform,
                     [[maybe_unused]] JPH::Vec3Arg scale,
                     [[maybe_unused]] JPH::ColorArg color,
                     [[maybe_unused]] bool useMaterialColors,
                     [[maybe_unused]] bool drawWireframe ) const {}
#endif

bool SdfShape::CastRay( const JPH::RayCast& ray,
                        const JPH::SubShapeIDCreator& subShapeIdCreator,
                        JPH::RayCastResult& outHit ) const {
    const Vector3f32 rayOrigin{ ray.mOrigin.GetX(), ray.mOrigin.GetY(), ray.mOrigin.GetZ() };
    Vector3f32 rayDirection{ ray.mDirection.GetX(), ray.mDirection.GetY(), ray.mDirection.GetZ() };

    const float32 maxDistance = numericCast< float32 >( rayDirection.lengthSquared() );
    rayDirection.normalize();

    Vector4f32 sample = m_sdf( rayOrigin );
    if( ( sample.W * sample.W ) > maxDistance ) {
        return false;
    }

    float32 t = sample.W;

    constexpr uint32_t MaxSteps = 16;
    for( uint32_t step = 0; step < MaxSteps; ++step ) {
        const Vector3f32 position = rayOrigin + rayDirection * t;
        const Vector4f32 sample = m_sdf( position );

        if( sample.W <= 0.001f ) {
            float normalizedHitDistance = t / maxDistance;
            if( normalizedHitDistance < outHit.mFraction ) {
                outHit.mFraction = normalizedHitDistance;
                outHit.mSubShapeID2 = subShapeIdCreator.GetID();
                return true;
            } else {
                return false;
            }
        }

        t += sample.W;
        if( t >= maxDistance ) {
            return false;
        }
    }

    return false;
}

void SdfShape::CastRay( const JPH::RayCast& ray,
                        const JPH::RayCastSettings& rayCastSettings,
                        const JPH::SubShapeIDCreator& subShapeIDCreator,
                        JPH::CastRayCollector& outCollector,
                        const JPH::ShapeFilter& shapeFilter ) const {
    JPH::RayCastResult hitResult;
    hitResult.mFraction = outCollector.GetEarlyOutFraction();
    if( CastRay( ray, subShapeIDCreator, hitResult ) ) {
        outCollector.AddHit( hitResult );
    }
}

void SdfShape::CollidePoint( JPH::Vec3Arg point,
                             const JPH::SubShapeIDCreator& subShapeIDCreator,
                             JPH::CollidePointCollector& outCollector,
                             const JPH::ShapeFilter& shapeFilter ) const {
    Vector4f32 sample = m_sdf( { point.GetX(), point.GetY(), point.GetZ() } );
    if( sample.W <= 0.0f ) {
        JPH::CollidePointResult result{
            .mBodyID = JPH::BodyID(),
            .mSubShapeID2 = subShapeIDCreator.GetID(),
        };

        outCollector.AddHit( result );
    }
}

void SdfShape::CollideSoftBodyVertices( [[maybe_unused]] JPH::Mat44Arg centerOfMassTransform,
                                        [[maybe_unused]] JPH::Vec3Arg scale,
                                        [[maybe_unused]] const JPH::CollideSoftBodyVertexIterator& vertices,
                                        [[maybe_unused]] uint numVertices,
                                        [[maybe_unused]] int collidingShapeIndex ) const {}

void SdfShape::GetTrianglesStart( [[maybe_unused]] GetTrianglesContext& outContext,
                                  [[maybe_unused]] const JPH::AABox& box,
                                  [[maybe_unused]] JPH::Vec3Arg positionCOM,
                                  [[maybe_unused]] JPH::QuatArg rotation,
                                  [[maybe_unused]] JPH::Vec3Arg scale ) const {}

int SdfShape::GetTrianglesNext( [[maybe_unused]] GetTrianglesContext& outContext,
                                [[maybe_unused]] int maxTrianglesRequested,
                                [[maybe_unused]] JPH::Float3* outTriangleVertices,
                                [[maybe_unused]] const JPH::PhysicsMaterial** outMaterials ) const {
    return 0;
}

float32 SdfShape::GetVolume() const {
    return std::numeric_limits< float32 >::max();
}

Vector4f32 SdfShape::evaluateSdf( Vector3f32 position ) const {
    ONYX_ASSERT( m_sdf != nullptr );
    return m_sdf( position );
}
} // namespace onyx::physics::jolt
