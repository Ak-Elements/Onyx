#include <onyx/physics/jolt/joltsdfcollision.h>

#include <onyx/physics/jolt/joltsdfshape.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/CollisionDispatch.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/TransformedShape.h>

namespace onyx::physics::jolt::sdf {

float32 sphereCast( const JPH::Vec3& castOrigin,
                    const JPH::Vec3& castDirection,
                    float32 sphereRadius,
                    const SdfShape& sdfShape ) {
    float32 maxDistance = castDirection.Length();
    if( maxDistance < std::numeric_limits< float32 >::epsilon() ) {
        return -1.0f;
    }

    JPH::Vec3 directionNormalized = castDirection / maxDistance;
    float32 t = 0.0f;
    constexpr uint32_t MaxSteps = 16;
    for( uint32_t i = 0; i < MaxSteps; ++i ) {
        JPH::Vec3 position = castOrigin + directionNormalized * t;
        Vector4f32 sample = sdfShape.evaluateSdf( { position.GetX(), position.GetY(), position.GetZ() } );
        float32 distance = sample.W - sphereRadius;

        if( distance <= 0.0f )
            return t / maxDistance;

        t += distance;
        if( t >= maxDistance ) {
            break;
        }
    }

    return -1.0f;
}

void castSphere( const JPH::ShapeCast& shapeCast,
                 const JPH::ShapeCastSettings& settings,
                 const JPH::Shape* sdfShapeBase,
                 const JPH::Vec3 scale, // NOLINT
                 const JPH::ShapeFilter& filter,
                 const JPH::Mat44& transform,
                 const JPH::SubShapeIDCreator& lhsIdCreator,
                 const JPH::SubShapeIDCreator& rhsIdCreator,
                 JPH::CastShapeCollector& outCollector ) {
    const SdfShape* sdfShape = static_cast< const SdfShape* >( sdfShapeBase );
    const JPH::SphereShape* sphereShape = static_cast< const JPH::SphereShape* >( shapeCast.mShape );

    JPH::Vec3 shapeCastOrigin = shapeCast.mCenterOfMassStart.GetTranslation();
    JPH::Vec3 shapeCastDirection = shapeCast.mDirection; // not normalized

    const float32 sphereRadius = sphereShape->GetRadius();
    float32 normalizedHitDistance = sphereCast( shapeCastOrigin, shapeCastDirection, sphereRadius, *sdfShape );
    if( normalizedHitDistance < 0.0f ) {
        return;
    }

    JPH::Vec3 hit = shapeCastOrigin + shapeCastDirection * normalizedHitDistance;
    Vector4f32 hitSample = sdfShape->evaluateSdf( { hit.GetX(), hit.GetY(), hit.GetZ() } );
    JPH::Vec3 hitNormalLocalSpace{ hitSample.X, hitSample.Y, hitSample.Z };
    const float32 dot = shapeCastDirection.Dot( hitNormalLocalSpace );
    const float32 penetrationDepth = -hitSample.W + sphereRadius;

    JPH::Vec3 contact1;
    JPH::Vec3 contact2;

    // inside
    if( penetrationDepth > 0.0f ) {
        if( penetrationDepth <= -outCollector.GetEarlyOutFraction() ) {
            return;
        }

        contact1 = transform * ( hit - hitNormalLocalSpace * sphereRadius );
        contact2 = transform * ( hit - hitNormalLocalSpace * hitSample.W );
    }

    else if( dot < 0.0f ) {
        if( normalizedHitDistance >= outCollector.GetEarlyOutFraction() ) {
            return;
        }

        contact1 = transform * ( hit - hitNormalLocalSpace * sphereRadius );
        contact2 = transform * ( hit - hitNormalLocalSpace * hitSample.W );
    } else {
        return;
    }

    JPH::ShapeCastResult result{ normalizedHitDistance,
                                 contact1,
                                 contact2,
                                 -transform.Multiply3x3( hitNormalLocalSpace ),
                                 false,
                                 lhsIdCreator.GetID(),
                                 rhsIdCreator.GetID(),
                                 JPH::TransformedShape::sGetBodyID( outCollector.GetContext() ) };

    outCollector.AddHit( result );
}

JPH::Vec3 closestPointOnBox( const JPH::Vec3 position,
                             const JPH::Vec3 boxCenter,
                             const JPH::Mat44 boxRotation,
                             const JPH::Vec3 boxHalfExtents ) {
    JPH::Vec3 positionInBoxSpace = boxRotation.Inversed() * ( position - boxCenter );
    JPH::Vec3 clamped{ std::clamp( positionInBoxSpace.GetX(), -boxHalfExtents.GetX(), boxHalfExtents.GetX() ),
                       std::clamp( positionInBoxSpace.GetY(), -boxHalfExtents.GetY(), boxHalfExtents.GetY() ),
                       std::clamp( positionInBoxSpace.GetZ(), -boxHalfExtents.GetZ(), boxHalfExtents.GetZ() ) };

    return boxCenter + boxRotation * clamped;
}

JPH::Vec3 closestPointOnSegment( JPH::Vec3 position, JPH::Vec3 segmentStart, JPH::Vec3 segmentEnd ) {
    const JPH::Vec3 direction = segmentEnd - segmentStart;
    float32 t = ( position - segmentStart ).Dot( direction );
    const float32 distanceSquared = direction.Dot( direction );

    if( distanceSquared < std::numeric_limits< float32 >::epsilon() ) {
        return segmentStart;
    }

    t = std::clamp( t / distanceSquared, 0.0f, 1.0f );
    return segmentStart + direction * t;
}

void collideSphere( const JPH::Shape* lhsShape,
                    const JPH::Shape* rhsShape,
                    const JPH::Vec3 lhsScale,
                    const JPH::Vec3 rhsScale,
                    const JPH::Mat44& lhsTransform,
                    const JPH::Mat44& rhsTransform,
                    const JPH::SubShapeIDCreator& lhsIdCreator,
                    const JPH::SubShapeIDCreator& rhsIdCreator,
                    const JPH::CollideShapeSettings& settings,
                    JPH::CollideShapeCollector& outCollector,
                    const JPH::ShapeFilter& inShapeFilter ) {
    const SdfShape* sdfShape = static_cast< const SdfShape* >( lhsShape );
    const JPH::SphereShape* sphereShape = static_cast< const JPH::SphereShape* >( rhsShape );

    JPH::Mat44 toLocal = lhsTransform.Inversed();
    JPH::Vec3 sphereCenterLocalSpace = toLocal * rhsTransform.GetTranslation();

    const float32 radius = sphereShape->GetRadius() * rhsScale.GetX();

    const Vector4f32 sample = sdfShape->evaluateSdf(
        { sphereCenterLocalSpace.GetX(), sphereCenterLocalSpace.GetY(), sphereCenterLocalSpace.GetZ() } );

    float32 penetrationDepth = -sample.W + radius;
    if( penetrationDepth <= -settings.mMaxSeparationDistance ) {
        return;
    }

    const JPH::Vec3 hitNormalLocalSpace{ sample.X, sample.Y, sample.Z };
    const JPH::Vec3 hitNormalWorldSpace = lhsTransform.Multiply3x3( hitNormalLocalSpace ).Normalized();
    const JPH::Vec3 contactOnSdfLocalSpace = sphereCenterLocalSpace - hitNormalLocalSpace * sample.W;
    const JPH::Vec3 contact0 = lhsTransform * contactOnSdfLocalSpace;

    const JPH::Vec3 sphereCenterWorldSpace = rhsTransform.GetTranslation();
    const JPH::Vec3 contact1 = sphereCenterWorldSpace - hitNormalWorldSpace * radius;

    JPH::CollideShapeResult contact{ contact0,
                                     contact1,
                                     hitNormalWorldSpace,
                                     penetrationDepth,
                                     lhsIdCreator.GetID(),
                                     rhsIdCreator.GetID(),
                                     JPH::TransformedShape::sGetBodyID( outCollector.GetContext() ) };
    outCollector.AddHit( contact );
}

void collideBox( const JPH::Shape* lhsShape,
                 const JPH::Shape* rhsShape,
                 const JPH::Vec3 lhsScale,
                 const JPH::Vec3 rhsScale,
                 const JPH::Mat44& lhsTransform,
                 const JPH::Mat44& rhsTransform,
                 const JPH::SubShapeIDCreator& lhsIdCreator,
                 const JPH::SubShapeIDCreator& rhsIdCreator,
                 const JPH::CollideShapeSettings& settings,
                 JPH::CollideShapeCollector& outCollector,
                 const JPH::ShapeFilter& inShapeFilter ) {
    const SdfShape* sdfShape = static_cast< const SdfShape* >( lhsShape );
    const JPH::BoxShape* boxShape = static_cast< const JPH::BoxShape* >( rhsShape );

    const JPH::Mat44 toLocal = lhsTransform.Inversed();
    const JPH::Mat44 boxTransformLocalSpace = toLocal * rhsTransform;
    const JPH::Vec3 boxCenterLocalSpace = boxTransformLocalSpace.GetTranslation();
    const JPH::Mat44 boxRotationLocalSpace = boxTransformLocalSpace.GetRotation();

    const JPH::Vec3 boxHalfExtents = boxShape->GetHalfExtent();

    const float32 boundingRadiusSquared = boxHalfExtents.LengthSq();
    const Vector4f32 centerSample = sdfShape->evaluateSdf(
        { boxCenterLocalSpace.GetX(), boxCenterLocalSpace.GetY(), boxCenterLocalSpace.GetZ() } );
    if( centerSample.W * centerSample.W >
        ( boundingRadiusSquared + ( settings.mCollisionTolerance * settings.mCollisionTolerance ) ) ) {
        return;
    }

    const JPH::Vec3 centerGradient{ centerSample.X, centerSample.Y, centerSample.Z };

    JPH::Vec3 axes[ 3 ] = {
        boxRotationLocalSpace.GetColumn3( 0 ),
        boxRotationLocalSpace.GetColumn3( 1 ),
        boxRotationLocalSpace.GetColumn3( 2 ),
    };

    float32 projected[ 3 ]{
        centerGradient.Dot( axes[ 0 ] ),
        centerGradient.Dot( axes[ 1 ] ),
        centerGradient.Dot( axes[ 2 ] ),
    };

    const JPH::Vec3 predictedCollisionOffset = axes[ 0 ] * ( -projected[ 0 ] * boxHalfExtents.GetX() ) +
                                               axes[ 1 ] * ( -projected[ 1 ] * boxHalfExtents.GetY() ) +
                                               axes[ 2 ] * ( -projected[ 2 ] * boxHalfExtents.GetZ() );

    const JPH::Vec3 predictedCollision = boxCenterLocalSpace + predictedCollisionOffset;

    DynamicArray< JPH::Vec3 > possibleContacts{
        predictedCollision,
        boxCenterLocalSpace,
        boxCenterLocalSpace - axes[ 0 ] * ( std::copysign( boxHalfExtents[ 0 ], projected[ 0 ] ) ),
        boxCenterLocalSpace - axes[ 1 ] * ( std::copysign( boxHalfExtents[ 1 ], projected[ 1 ] ) ),
        boxCenterLocalSpace - axes[ 2 ] * ( std::copysign( boxHalfExtents[ 2 ], projected[ 2 ] ) ),
    };

    struct Contact {
        JPH::Vec3 Normal;
        float32 Depth;
        JPH::Vec3 PointOnSdf;
        JPH::Vec3 PointOnBox;
    };

    DynamicArray< Contact > contacts;
    contacts.reserve( 5 );

    constexpr float32 AcceptableDifference = 0.98f;
    for( const JPH::Vec3& position : possibleContacts ) {
        const Vector4f32 sample = sdfShape->evaluateSdf( { position.GetX(), position.GetY(), position.GetZ() } );
        if( sample.W >= 0.0f ) {
            continue;
        }

        float32 depth = -sample.W;
        JPH::Vec3 hitNormal{ sample.X, sample.Y, sample.Z };

        bool skipContact = false;
        for( Contact& contact : contacts ) {
            if( contact.Normal.Dot( hitNormal ) > AcceptableDifference ) {
                if( depth > contact.Depth ) {
                    contact.Depth = depth;
                    contact.Normal = hitNormal;
                    contact.PointOnSdf = lhsTransform * ( position - hitNormal * sample.W );
                    contact.PointOnBox = lhsTransform * closestPointOnBox( position - hitNormal * sample.W,
                                                                           boxCenterLocalSpace,
                                                                           boxRotationLocalSpace,
                                                                           boxHalfExtents );
                }
                skipContact = true;
                break;
            }
        }

        if( skipContact ) {
            continue;
        }

        contacts.push_back( { hitNormal,
                              depth,
                              lhsTransform * ( position - hitNormal * sample.W ),
                              lhsTransform * closestPointOnBox( position - hitNormal * sample.W,
                                                                boxCenterLocalSpace,
                                                                boxRotationLocalSpace,
                                                                boxHalfExtents ) } );
    }

    for( const Contact& contact : contacts ) {
        if( outCollector.ShouldEarlyOut() ) {
            break;
        }

        JPH::CollideShapeResult result;
        result.mPenetrationDepth = contact.Depth;
        result.mPenetrationAxis = lhsTransform.Multiply3x3( contact.Normal ).Normalized();
        result.mContactPointOn1 = contact.PointOnSdf;
        result.mContactPointOn2 = contact.PointOnBox;
        result.mSubShapeID1 = lhsIdCreator.GetID();
        result.mSubShapeID2 = rhsIdCreator.GetID();

        outCollector.AddHit( result );
    }
}

// void castBox( const JPH::ShapeCast& shapeCast,
//               const JPH::ShapeCastSettings& settings,
//               const JPH::Shape* sdfShapeBase,
//               const JPH::Vec3 scale, // NOLINT
//               const JPH::ShapeFilter& filter,
//               const JPH::Mat44& transform,
//               const JPH::SubShapeIDCreator& lhsIdCreator,
//               const JPH::SubShapeIDCreator& rhsIdCreator,
//               JPH::CastShapeCollector& outCollector ) {}

void collideCapsule( const JPH::Shape* lhsShape,
                     const JPH::Shape* rhsShape,
                     const JPH::Vec3 lhsScale,
                     const JPH::Vec3 rhsScale,
                     const JPH::Mat44& lhsTransform,
                     const JPH::Mat44& rhsTransform,
                     const JPH::SubShapeIDCreator& lhsIdCreator,
                     const JPH::SubShapeIDCreator& rhsIdCreator,
                     const JPH::CollideShapeSettings& settings,
                     JPH::CollideShapeCollector& outCollector,
                     const JPH::ShapeFilter& inShapeFilter ) {
    const SdfShape* sdfShape = static_cast< const SdfShape* >( lhsShape );
    const JPH::CapsuleShape* capsuleShape = static_cast< const JPH::CapsuleShape* >( rhsShape );

    const float32 radius = capsuleShape->GetRadius();
    const float32 halfHeight = capsuleShape->GetHalfHeightOfCylinder();

    const JPH::Mat44 toLocal = lhsTransform.Inversed();
    const JPH::Mat44 capsuleTransformLocalSpace = toLocal * rhsTransform;
    const JPH::Vec3 capsuleCenterLocalSpace = capsuleTransformLocalSpace.GetTranslation();
    const JPH::Mat44 capsuleRotationLocalSpace = capsuleTransformLocalSpace.GetRotation();
    const JPH::Vec3 spineDirection = capsuleRotationLocalSpace.GetColumn3( 1 ).Normalized();

    const JPH::Vec3 segmentBottom = capsuleCenterLocalSpace - spineDirection * halfHeight;
    const JPH::Vec3 segmentTop = capsuleCenterLocalSpace + spineDirection * halfHeight;

    const float32 boundingRadiusSquared = ( radius + halfHeight + settings.mMaxSeparationDistance ) *
                                          ( radius + halfHeight + settings.mMaxSeparationDistance );
    const Vector4f32 centerSample = sdfShape->evaluateSdf(
        { capsuleCenterLocalSpace.GetX(), capsuleCenterLocalSpace.GetY(), capsuleCenterLocalSpace.GetZ() } );
    if( ( centerSample.W * centerSample.W ) > boundingRadiusSquared ) {
        return;
    }

    const JPH::Vec3 centerGradient{ centerSample.X, centerSample.Y, centerSample.Z };
    const JPH::Vec3 predictedSurface = capsuleCenterLocalSpace - centerGradient * centerSample.W;
    const float t = std::clamp( ( predictedSurface - segmentBottom ).Dot( spineDirection ) / ( 2.0f * halfHeight ),
                                0.0f,
                                1.0f );

    const JPH::Vec3 closestPointToSpine = segmentBottom + ( segmentTop - segmentBottom ) * t;

    const Vector4f32 sample = sdfShape->evaluateSdf(
        { closestPointToSpine.GetX(), closestPointToSpine.GetY(), closestPointToSpine.GetZ() } );
    const Vector4f32 sampleBottom = sdfShape->evaluateSdf(
        { segmentBottom.GetX(), segmentBottom.GetY(), segmentBottom.GetZ() } );
    const Vector4f32 sampleTop = sdfShape->evaluateSdf( { segmentTop.GetX(), segmentTop.GetY(), segmentTop.GetZ() } );

    struct ContactCandiate {
        JPH::Vec3 Position;
        Vector4f32 Sample;
    };

    ContactCandiate candiates[ 3 ]{ { closestPointToSpine, sample },
                                    { segmentBottom, sampleBottom },
                                    { segmentTop, sampleTop } };

    JPH::CollideShapeResult bestCandiate;
    bestCandiate.mPenetrationDepth = std::numeric_limits< float32 >::lowest();
    bool hasContact = false;

    for( auto& [ position, sample ] : candiates ) {
        const float32 penetrationDepth = -sample.W + radius;
        if( penetrationDepth <= -settings.mMaxSeparationDistance ) {
            continue;
        }

        if( penetrationDepth <= bestCandiate.mPenetrationDepth ) {
            continue;
        }

        const JPH::Vec3 hitNormalLocalSpace{ sample.X, sample.Y, sample.Z };
        const JPH::Vec3 hitNormalWorldSpace = lhsTransform.Multiply3x3( hitNormalLocalSpace ).Normalized();
        const JPH::Vec3 contactOnSdfLocalSpace = position - hitNormalLocalSpace * sample.W;
        const JPH::Vec3 contact0 = lhsTransform * contactOnSdfLocalSpace;
        const JPH::Vec3 contactOnCapsuleLocalSpace = position - hitNormalLocalSpace * radius;
        const JPH::Vec3 contact1 = lhsTransform * contactOnCapsuleLocalSpace;
        bestCandiate = JPH::CollideShapeResult{ contact0,
                                                contact1,
                                                hitNormalWorldSpace,
                                                penetrationDepth,
                                                lhsIdCreator.GetID(),
                                                rhsIdCreator.GetID(),
                                                JPH::TransformedShape::sGetBodyID( outCollector.GetContext() ) };

        hasContact = true;
    }

    if( hasContact )
        outCollector.AddHit( bestCandiate );
}

//
void castCapsule( const JPH::ShapeCast& shapeCast,
                  const JPH::ShapeCastSettings& settings,
                  const JPH::Shape* sdfShapeBase,
                  const JPH::Vec3 scale, // NOLINT
                  const JPH::ShapeFilter& filter,
                  const JPH::Mat44& sdfCenterOfMassTransform,
                  const JPH::SubShapeIDCreator& shapeCastIdCreator,
                  const JPH::SubShapeIDCreator& sdfIdCreator,
                  JPH::CastShapeCollector& outCollector ) {
    const SdfShape* sdfShape = static_cast< const SdfShape* >( sdfShapeBase );
    const JPH::CapsuleShape* capsuleShape = static_cast< const JPH::CapsuleShape* >( shapeCast.mShape );
    const float32 maxDistance = shapeCast.mDirection.Length();

    const float32 radius = capsuleShape->GetRadius();
    const float32 halfHeight = capsuleShape->GetHalfHeightOfCylinder();

    const JPH::Mat44 capsuleTransformLocalSpace = shapeCast.mCenterOfMassStart;
    const JPH::Vec3 capsuleCenterLocalSpace = capsuleTransformLocalSpace.GetTranslation();
    const JPH::Mat44 capsuleRotationLocalSpace = capsuleTransformLocalSpace.GetRotation();
    const JPH::Vec3 spineDirection = capsuleRotationLocalSpace.GetColumn3( 1 ).Normalized();
    const JPH::Vec3 castDirectionLocalSpace = shapeCast.mDirection.Normalized();

    const float32 boundingRadius = radius + halfHeight;

    float t = 0.0f;
    constexpr uint32_t MaxSteps = 16;
    for( uint32_t i = 0; i < MaxSteps; ++i ) {
        const JPH::Vec3 currentPosition = capsuleCenterLocalSpace + castDirectionLocalSpace * t;
        Vector4f32 hitPointSample = sdfShape->evaluateSdf(
            { currentPosition.GetX(), currentPosition.GetY(), currentPosition.GetZ() } );
        JPH::Vec3 sampleGradient{ hitPointSample.X, hitPointSample.Y, hitPointSample.Z };

        float distanceToSurface = hitPointSample.W;
        if( ( distanceToSurface - boundingRadius ) > ( maxDistance - t ) ) {
            return;
        }

        if( t > maxDistance )
            return;

        const float support = radius + halfHeight * std::abs( spineDirection.Dot( sampleGradient ) );
        if( std::abs( distanceToSurface ) > support ) {
            t += distanceToSurface - support;
            continue;
        }
        const JPH::Vec3 segmentBottom = currentPosition - spineDirection * halfHeight;
        const JPH::Vec3 segmentTop = currentPosition + spineDirection * halfHeight;

        const JPH::Vec3 predictedSurfacePosition = currentPosition - sampleGradient * hitPointSample.W;
        const float deltaT = std::clamp( ( predictedSurfacePosition - segmentBottom ).Dot( spineDirection ) /
                                             ( 2.0f * halfHeight ),
                                         0.0f,
                                         1.0f );
        const JPH::Vec3 closestToSpine = segmentBottom + ( segmentTop - segmentBottom ) * deltaT;

        hitPointSample = sdfShape->evaluateSdf(
            { closestToSpine.GetX(), closestToSpine.GetY(), closestToSpine.GetZ() } );
        const JPH::Vec3 hitNormal{ hitPointSample.X, hitPointSample.Y, hitPointSample.Z };

        const float penetrationDepth = -hitPointSample.W + radius;
        const float32 dot = castDirectionLocalSpace.Dot( hitNormal );
        float fraction = t / maxDistance;

        // inside sdf
        if( penetrationDepth > 0.0f ) {
            if( penetrationDepth <= -outCollector.GetEarlyOutFraction() ) {
                return;
            }

            fraction = 0.0f;
        } else if( dot < 0.0f ) { // moving towards
            if( fraction >= outCollector.GetEarlyOutFraction() ) {
                return;
            }
        } else { // moving away
            return;
        }

        const JPH::Vec3 pointOnSdf = closestToSpine - hitNormal * hitPointSample.W;
        const JPH::Vec3 closestPoint = closestPointOnSegment( pointOnSdf, segmentBottom, segmentTop );
        const JPH::Vec3 pointOnCapsule = closestPoint - hitNormal * radius;

        JPH::ShapeCastResult result{ fraction,
                                     sdfCenterOfMassTransform * pointOnCapsule,
                                     sdfCenterOfMassTransform * pointOnSdf,
                                     -sdfCenterOfMassTransform.Multiply3x3( hitNormal ).Normalized(),
                                     false,
                                     shapeCastIdCreator.GetID(),
                                     sdfIdCreator.GetID(),
                                     JPH::TransformedShape::sGetBodyID( outCollector.GetContext() ) };

        outCollector.AddHit( result );
        break;
    }
}

} // namespace onyx::physics::jolt::sdf
