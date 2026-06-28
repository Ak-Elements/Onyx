#pragma once

#include <onyx/volume/source/volumebase.h>

#include <onyx/geometry/vector3.h>
#include <onyx/geometry/vector4.h>

namespace onyx::volume {
class CSGPlane : public VolumeBase {
  public:
    CSGPlane() = default;
    CSGPlane( const float32 distance, const Vector3f32& normal )
        : m_distance( distance )
        , m_normal( normal ) {}

    [[nodiscard]] Vector4f32 GetValueAndGradient( const Vector3f32& position ) const override {
        return { m_normal.X, m_normal.Y, m_normal.Z, static_cast< float32 >( m_normal.dot( position ) - m_distance ) };
    }

    [[nodiscard]] float32 GetValue( const Vector3f32& position ) const override {
        return m_distance - static_cast< float32 >( m_normal.dot( position ) );
    }

    [[nodiscard]] float32 getDistance() const { return m_distance; }
    void setDistance( float32 distance ) { m_distance = distance; }

    [[nodiscard]] const Vector3f32& getNormal() const { return m_normal; }
    void setNormal( const Vector3f32& normal ) { m_normal = normal; }

  protected:
    float32 m_distance = 0.0f;
    Vector3f32 m_normal = Vector3f32::yUnit();
};
} // namespace onyx::volume
