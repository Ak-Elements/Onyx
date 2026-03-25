#include <onyx/volume/source/csg/operations/csgscale.h>

namespace onyx::volume {
CSGScale::CSGScale()
    : m_Scale( 1.0f ) {}

CSGScale::CSGScale( const VolumeBase* volume, const float32 scale )
    : CSGUnary( volume )
    , m_Scale( scale ) {}

float32 CSGScale::GetValue( const Vector3f32& /*position*/ ) const {
    return m_Scale;
}

Vector4f32 CSGScale::GetValueAndGradient( const Vector3f32& /*position*/ ) const {
    return Vector4f32();
}
} // namespace onyx::volume