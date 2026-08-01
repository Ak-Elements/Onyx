#pragma once

namespace onyx::volume {
class VolumeBase {
  public:
    virtual ~VolumeBase() = default;

    [[nodiscard]] virtual Vector4f32 getValueAndGradient( const Vector3f32& position ) const = 0;
    [[nodiscard]] virtual float32 getValue( const Vector3f32& position ) const = 0;
};

} // namespace onyx::volume
