#pragma once

#include <onyx/volume/source/volumebase.h>

namespace Onyx::Volume
{
    class CSGSphere : public VolumeBase
    {
    public:
        CSGSphere();
        CSGSphere(const onyxF32 radius, const Vector3f32& center);

        virtual Vector4f32 GetValueAndGradient(const Vector3f32& position) const override;

        virtual onyxF32 GetValue(const Vector3f32& position) const override;

        void SetRadius(onyxF32 r) { m_Radius = r; }
        void SetCenter(const Vector3f32& position) { m_Center = position; }

        const Vector3f32& GetCenter() const { return m_Center; }
        onyxF32 GetRadius() const { return m_Radius; }

    protected:
        onyxF32 m_Radius = 0.0f;
        Vector3f32 m_Center = { 0.0f, 0.0f, 0.0f };
    };
}

