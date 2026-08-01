#pragma once

#include <onyx/volume/source/volumebase.h>

#include <algorithm>
#include <onyx/geometry/vector3.h>
#include <onyx/geometry/vector4.h>

namespace onyx::volume {
class CSGCube : public VolumeBase {
  public:
    CSGCube()
        : m_center()
        , m_halfExtents( 0.5f ) {}

    CSGCube( const Vector3f32& center, const Vector3f32& halfExtents )
        : m_center( center )
        , m_halfExtents( halfExtents ) {}

    [[nodiscard]] Vector4f32 getValueAndGradient( const Vector3f32& position ) const override {
        Vector3f32 gradient( getValue( Vector3f32( position[ 0 ] + 1.0f, position[ 1 ], position[ 2 ] ) ) -
                                 getValue( Vector3f32( position[ 0 ] - 1.0f, position[ 1 ], position[ 2 ] ) ),
                             getValue( Vector3f32( position[ 0 ], position[ 1 ] + 1.0f, position[ 2 ] ) ) -
                                 getValue( Vector3f32( position[ 0 ], position[ 1 ] - 1.0f, position[ 2 ] ) ),
                             getValue( Vector3f32( position[ 0 ], position[ 1 ], position[ 2 ] + 1.0f ) ) -
                                 getValue( Vector3f32( position[ 0 ], position[ 1 ], position[ 2 ] - 1.0f ) ) );

        if( gradient.isZero() == false )
            gradient.normalize();
        gradient *= -1.0f;
        return { gradient[ 0 ], gradient[ 1 ], gradient[ 2 ], getDistanceTo( position ) };
    }

    [[nodiscard]] float32 getValue( const Vector3f32& position ) const override { return getDistanceTo( position ); }

    [[nodiscard]] Vector3f32 getCenter() const { return m_center; }
    void setCenter( const Vector3f32& center ) { m_center = center; }

    [[nodiscard]] Vector3f32 getHalfExtents() const { return m_halfExtents; }
    void setHalfExtents( const Vector3f32& halfExtents ) { m_halfExtents = halfExtents; }

  protected:
    [[nodiscard]] float32 getDistanceTo( const Vector3f32& position ) const {
        using std::abs;
        using std::max;
        using std::min;

        float32 x = max( position[ 0 ] - m_center[ 0 ] - m_halfExtents[ 0 ],
                         m_center[ 0 ] - position[ 0 ] - m_halfExtents[ 0 ] );

        float32 y = max( position[ 1 ] - m_center[ 1 ] - m_halfExtents[ 1 ],
                         m_center[ 1 ] - position[ 1 ] - m_halfExtents[ 1 ] );

        float32 z = max( position[ 2 ] - m_center[ 2 ] - m_halfExtents[ 2 ],
                         m_center[ 2 ] - position[ 2 ] - m_halfExtents[ 2 ] );

        float32 d = x;
        d = max( d, y );
        d = max( d, z );

        Vector3f32 distance( x, y, z );
        return d <= 0 ? distance.length() : -distance.length();
    }

  protected:
    Vector3f32 m_center;
    Vector3f32 m_halfExtents;
};
} // namespace onyx::volume
