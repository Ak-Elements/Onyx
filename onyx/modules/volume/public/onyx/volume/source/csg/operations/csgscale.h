#pragma once

#include <onyx/volume/source/csg/operations/csgunary.h>

namespace Onyx::Volume
{
    class CSGScale : public CSGUnary
    {
    public:
        explicit CSGScale(const VolumeBase* volume, const onyxF32 scale);
        CSGScale();

        virtual Vector4f GetValueAndGradient(const Vector3f& position) const override;
        virtual onyxF32 GetValue(const Vector3f& position) const override;
    private:
        onyxF32 m_Scale;
    };
}