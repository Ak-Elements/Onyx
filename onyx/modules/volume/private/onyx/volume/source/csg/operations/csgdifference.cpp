#include <onyx/volume/source/csg/operations/csgdifference.h>

namespace onyx::volume {
CSGDifference::CSGDifference() {}

CSGDifference::CSGDifference( VolumeBase* first, VolumeBase* second )
    : CSGOperation( first, second ) {}

Vector4f32 CSGDifference::getValueAndGradient( const Vector3f32& position ) const {
    if( m_First && m_Second ) {
        Vector4f32 valueA = m_First->getValueAndGradient( position );
        Vector4f32 valueB = -1.0f * m_Second->getValueAndGradient( position );
        if( valueA[ 3 ] < valueB[ 3 ] ) {
            return valueA;
        }
        return valueB;
    }

    return Vector4f32( std::numeric_limits< float32 >::max() );
}

float32 CSGDifference::getValue( const Vector3f32& position ) const {
    if( m_First && m_Second ) {
        float32 valueA = m_First->getValue( position );
        float32 valueB = -1.0f * m_Second->getValue( position );
        if( valueA < valueB ) {
            return valueA;
        }
        return valueB;
    }

    return std::numeric_limits< float32 >::max();
}

} // namespace onyx::volume
