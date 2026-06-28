#pragma once

#include <onyx/volume/source/volumebase.h>

namespace onyx::volume {
class CSGSphere : public VolumeBase {
  public:
    CSGSphere() = default;
    CSGSphere( float32 radius, const Vector3f32& center );

    [[nodiscard]] Vector4f32 GetValueAndGradient( const Vector3f32& position ) const override;
    [[nodiscard]] float32 GetValue( const Vector3f32& position ) const override;

    float32 Radius = 0.5f;
    Vector3f32 Center = { 0.0f, 0.0f, 0.0f };
};
} // namespace onyx::volume
