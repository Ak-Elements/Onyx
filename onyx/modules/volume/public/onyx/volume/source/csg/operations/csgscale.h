#pragma once

#include <onyx/volume/source/csg/operations/csgunary.h>

namespace onyx::volume {
class CSGScale : public CSGUnary {
  public:
    explicit CSGScale( const VolumeBase* volume, const float32 scale );
    CSGScale();

    virtual Vector4f32 GetValueAndGradient( const Vector3f32& position ) const override;
    virtual float32 GetValue( const Vector3f32& position ) const override;

  private:
    float32 m_Scale;
};
} // namespace onyx::volume