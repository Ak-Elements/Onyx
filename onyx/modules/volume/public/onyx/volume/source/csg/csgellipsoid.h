#pragma once

#include <onyx/volume/source/volumebase.h>

namespace onyx::volume {
class CsgEllipsoid : public VolumeBase {
  public:
    CsgEllipsoid() = default;
    CsgEllipsoid( Vector3f32 center, Vector3f32 radii );

    [[nodiscard]] Vector4f32 getValueAndGradient( const Vector3f32& position ) const override;
    [[nodiscard]] float32 getValue( const Vector3f32& position ) const override;

    Vector3f32 Center = { 0.0f, 0.0f, 0.0f };
    Vector3f32 Radii = { 0.0f, 0.0f, 0.0f };
};
} // namespace onyx::volume
