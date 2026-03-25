#include <onyx/volume/source/csg/operations/csgunion.h>

namespace onyx::volume {
CSGUnion::CSGUnion() {}

CSGUnion::CSGUnion( VolumeBase* first, VolumeBase* second )
    : CSGOperation( first, second ) {}

Vector4f32 CSGUnion::GetValueAndGradient( const Vector3f32& position ) const {
    if ( m_First && m_Second ) {
        Vector4f32 valueA = m_First->GetValueAndGradient( position );
        Vector4f32 valueB = m_Second->GetValueAndGradient( position );
        if ( valueA[ 3 ] > valueB[ 3 ] ) {
            return valueA;
        }
        return valueB;
    }

    return Vector4f32( std::numeric_limits< float32 >::max() );
}

float32 CSGUnion::GetValue( const Vector3f32& position ) const {
    if ( m_First && m_Second ) {
        float32 valueA = m_First->GetValue( position );
        float32 valueB = m_Second->GetValue( position );
        if ( valueA > valueB ) {
            return valueA;
        }
        return valueB;
    }

    return std::numeric_limits< float32 >::max();
}

} // namespace onyx::volume