#pragma once

#include <onyx/volume/source/csg/operations/csgunary.h>

namespace onyx::volume {
class CSGNegate : public CSGUnary {
  public:
    explicit CSGNegate( const VolumeBase* volume );
    CSGNegate();

    virtual Vector4f32 GetValueAndGradient( const Vector3f32& position ) const override;
    virtual float32 GetValue( const Vector3f32& position ) const override;
};
} // namespace onyx::volume