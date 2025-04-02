#pragma once

#include <onyx/volume/source/csg/operations/csgoperation.h>

namespace Onyx::Volume
{
    class CSGDifference : public CSGOperation
    {
    public:
        CSGDifference();
        CSGDifference(VolumeBase* first, VolumeBase* second);


        virtual Vector4f GetValueAndGradient(const Vector3f& position) const override;

        virtual onyxF32 GetValue(const Vector3f& position) const override;
    };
}