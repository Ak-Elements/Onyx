#pragma once

#include <onyx/volume/source/csg/operations/csgoperation.h>

namespace onyx::volume {
class CSGDifference : public CSGOperation {
  public:
    CSGDifference();
    CSGDifference( VolumeBase* first, VolumeBase* second );

    virtual Vector4f32 getValueAndGradient( const Vector3f32& position ) const override;

    virtual float32 getValue( const Vector3f32& position ) const override;
};
} // namespace onyx::volume
