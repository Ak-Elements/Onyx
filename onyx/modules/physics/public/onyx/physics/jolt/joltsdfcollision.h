#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/CollisionCollector.h>

namespace JPH { // NOLINT
class Shape;
class Vec3;
class Mat44;
class SubShapeIDCreator;
class CollideShapeSettings;
struct ShapeCast;
class ShapeCastSettings;
class ShapeFilter;
using CollideShapeCollector = CollisionCollector< class CollideShapeResult,
                                                  class CollisionCollectorTraitsCollideShape >;
using CastShapeCollector = CollisionCollector< class ShapeCastResult, class CollisionCollectorTraitsCastShape >;
} // namespace JPH

namespace onyx::physics::jolt::sdf {

void collideSphere( const JPH::Shape* lhsShape,
                    const JPH::Shape* rhsShape,
                    const JPH::Vec3 lhsScale, // NOLINT
                    const JPH::Vec3 rhsScale, // NOLINT
                    const JPH::Mat44& lhsTransform,
                    const JPH::Mat44& rhsTransform,
                    const JPH::SubShapeIDCreator& lhsIdCreator,
                    const JPH::SubShapeIDCreator& rhsIdCreator,
                    const JPH::CollideShapeSettings& settings,
                    JPH::CollideShapeCollector& outCollector,
                    const JPH::ShapeFilter& inShapeFilter );

void castSphere( const JPH::ShapeCast& shapeCast,
                 const JPH::ShapeCastSettings& settings,
                 const JPH::Shape* sdfShapeBase,
                 const JPH::Vec3 scale, // NOLINT
                 const JPH::ShapeFilter& filter,
                 const JPH::Mat44& transform,
                 const JPH::SubShapeIDCreator& lhsIdCreator,
                 const JPH::SubShapeIDCreator& rhsIdCreator,
                 JPH::CastShapeCollector& outCollector );

void collideBox( const JPH::Shape* lhsShape,
                 const JPH::Shape* rhsShape,
                 const JPH::Vec3 lhsScale, // NOLINT
                 const JPH::Vec3 rhsScale, // NOLINT
                 const JPH::Mat44& lhsTransform,
                 const JPH::Mat44& rhsTransform,
                 const JPH::SubShapeIDCreator& lhsIdCreator,
                 const JPH::SubShapeIDCreator& rhsIdCreator,
                 const JPH::CollideShapeSettings& settings,
                 JPH::CollideShapeCollector& outCollector,
                 const JPH::ShapeFilter& inShapeFilter );

void collideCapsule( const JPH::Shape* lhsShape,
                     const JPH::Shape* rhsShape,
                     const JPH::Vec3 lhsScale, // NOLINT
                     const JPH::Vec3 rhsScale, // NOLINT
                     const JPH::Mat44& lhsTransform,
                     const JPH::Mat44& rhsTransform,
                     const JPH::SubShapeIDCreator& lhsIdCreator,
                     const JPH::SubShapeIDCreator& rhsIdCreator,
                     const JPH::CollideShapeSettings& settings,
                     JPH::CollideShapeCollector& outCollector,
                     const JPH::ShapeFilter& inShapeFilter );

void collideCylinder( const JPH::Shape* lhsShape,
                      const JPH::Shape* rhsShape,
                      const JPH::Vec3 lhsScale, // NOLINT
                      const JPH::Vec3 rhsScale, // NOLINT
                      const JPH::Mat44& lhsTransform,
                      const JPH::Mat44& rhsTransform,
                      const JPH::SubShapeIDCreator& lhsIdCreator,
                      const JPH::SubShapeIDCreator& rhsIdCreator,
                      const JPH::CollideShapeSettings& settings,
                      JPH::CollideShapeCollector& outCollector );

void castCapsule( const JPH::ShapeCast& shapeCast,
                  const JPH::ShapeCastSettings& settings,
                  const JPH::Shape* sdfShapeBase,
                  const JPH::Vec3 scale, // NOLINT
                  const JPH::ShapeFilter& filter,
                  const JPH::Mat44& sdfCenterOfMassTransform,
                  const JPH::SubShapeIDCreator& shapeCastIdCreator,
                  const JPH::SubShapeIDCreator& sdfIdCreator,
                  JPH::CastShapeCollector& outCollector );

} // namespace onyx::physics::jolt::sdf
