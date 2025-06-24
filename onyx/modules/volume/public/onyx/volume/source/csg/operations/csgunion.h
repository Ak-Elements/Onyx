#pragma once

#include <onyx/volume/source/csg/operations/csgoperation.h>

namespace Onyx::Volume
{
    class CSGUnion : public CSGOperation
    {
    public:
        CSGUnion();
        CSGUnion(VolumeBase* first, VolumeBase* second);

        virtual Vector4f32 GetValueAndGradient(const Vector3f32& position) const override;

        virtual onyxF32 GetValue(const Vector3f32& position) const override;
    };
}