#include <onyx/volume/source/csg/csgellipsoid.h>

namespace onyx::volume {

CsgEllipsoid::CsgEllipsoid( Vector3f32 center, Vector3f32 radii )
    : Center( center )
    , Radii( radii ) {}

Vector4f32 CsgEllipsoid::getValueAndGradient( const Vector3f32& position ) const {
    const Vector3f32 difference = position - Center;
    const Vector3f32 radiiSquared{ Radii.X * Radii.X, Radii.Y * Radii.Y, Radii.Z * Radii.Z };

    const Vector3f32 k{
        difference.X / Radii.X,
        difference.Y / Radii.Y,
        difference.Z / Radii.Z,
    };
    const float k0 = k.length();

    Vector3f32 gradient{
        difference.X / radiiSquared.X,
        difference.Y / radiiSquared.Y,
        difference.Z / radiiSquared.Z,
    };

    float k1 = safeReciprocal( gradient.length() );
    return Vector4f32{ gradient * k1, k0 * ( k0 - 1.0f ) * k1 };
}

float32 CsgEllipsoid::getValue( const Vector3f32& position ) const {
    const Vector3f32 difference = position - Center;
    const Vector3f32 radiiSquared{ Radii.X * Radii.X, Radii.Y * Radii.Y, Radii.Z * Radii.Z };

    const Vector3f32 k{
        difference.X / Radii.X,
        difference.Y / Radii.Y,
        difference.Z / Radii.Z,
    };
    const float k0 = k.length();

    Vector3f32 gradient{
        difference.X / radiiSquared.X,
        difference.Y / radiiSquared.Y,
        difference.Z / radiiSquared.Z,
    };

    float k1 = safeReciprocal( gradient.length() );
    return k0 * ( k0 - 1.0f ) * k1;
}

} // namespace onyx::volume
