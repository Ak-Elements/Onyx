#pragma once

#include <onyx/volume/source/csg/operations/csgoperation.h>

namespace onyx::volume
{
    class CSGIntersect : public CSGOperation
    {
    public:
        CSGIntersect();
        CSGIntersect(VolumeBase* first, VolumeBase* second);

        virtual Vector4f32 GetValueAndGradient(const Vector3f32& position) const override;

        virtual onyxF32 GetValue(const Vector3f32& position) const override;
    };
}