#pragma once

#include <onyx/volume/source/volumebase.h>

namespace Onyx::Volume
{
    class CSGSphere : public VolumeBase
    {
    public:
        CSGSphere() = default;
        CSGSphere(onyxF32 radius, const Vector3f32& center);

        Vector4f32 GetValueAndGradient(const Vector3f32& position) const override;
        onyxF32 GetValue(const Vector3f32& position) const override;
    
        onyxF32 Radius = 0.5f;
        Vector3f32 Center = { 0.0f, 0.0f, 0.0f };
    };
}

