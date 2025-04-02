#pragma once

#include <onyx/volume/source/csg/operations/csgunary.h>

namespace Onyx::Volume
{
    class CSGNegate : public CSGUnary
    {
    public:
        explicit CSGNegate(const VolumeBase* volume);
        CSGNegate();

        virtual Vector4f GetValueAndGradient(const Vector3f& position) const override;
        virtual onyxF32 GetValue(const Vector3f& position) const override;
    };
}