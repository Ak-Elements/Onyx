#include <onyx/volume/source/csg/csgsphere.h>

namespace onyx::volume {

CSGSphere::CSGSphere( const float32 radius, const Vector3f32& center )
    : Radius( radius )
    , Center( center ) {}

Vector4f32 CSGSphere::GetValueAndGradient( const Vector3f32& position ) const {
    Vector3f32 gradient = position - Center;

    float32 length = 0.0f;
    if( gradient.isZero() == false ) {
        length = numericCast< float32 >( gradient.length() );
        if( length > 0.0f ) {
            gradient = gradient * ( 1.0f / length );
        }
    }

    return { gradient[ 0 ], gradient[ 1 ], gradient[ 2 ], length - Radius };
}

float32 CSGSphere::GetValue( const Vector3f32& position ) const {
    const Vector3f32 pMinCenter = position - Center;
    return Radius - numericCast< float32 >( pMinCenter.length() );
}

} // namespace onyx::volume
