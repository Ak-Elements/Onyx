#pragma once

#include <onyx/volume/source/csg/operations/csgoperation.h>

namespace onyx::volume {
class CSGUnion : public CSGOperation {
  public:
    CSGUnion();
    CSGUnion( VolumeBase* first, VolumeBase* second );

    [[nodiscard]] Vector4f32 GetValueAndGradient( const Vector3f32& position ) const override;

    [[nodiscard]] float32 GetValue( const Vector3f32& position ) const override;
};
} // namespace onyx::volume
